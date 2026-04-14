#include <iostream>
#include <condition_variable>
#include <atomic>
#include <thread>
#include <semaphore>
#include <vector>

std::vector<int> myVec{};// <6,7> 

std::counting_semaphore<1> signal2Ping(0);// <5,7>
std::counting_semaphore<1> signal2Pong(0);// <5> 

std::atomic<bool> atomicBool{};// <4>

std::atomic_flag condAtomicFlag1{}; // <2,3,6>
std::atomic_flag condAtomicFlag2{}; // <2>

bool dataReady{false}; // <0,1>

std::mutex mutex_;     // <0,1>
std::condition_variable condVar1;// <0,1>
std::condition_variable condVar2;// <1>

std::atomic<int> counter{};// <1,2,3,4,5>
constexpr int countlimit = 1'000'000;// <1,2,3,4,5>
//-----------------------------------------------
class Spinlock{
  std::atomic_flag flag = ATOMIC_FLAG_INIT;
public:

  void lock(){
    while( flag.test_and_set() );
  }

  void unlock(){
    flag.clear();
  }
  
};
Spinlock spin;
void workOnResource(){
    std::cout << "workOnResource" << std::endl;
    spin.lock();
    std::this_thread::sleep_for(std::chrono::milliseconds(2000));
    spin.unlock();
}
void spinLockSleep(){
    std::thread t1(workOnResource);
    std::thread t2(workOnResource);

    t1.join();
    t2.join();
}
std::mutex mut;
void workOnResourcem(){
    std::cout << "workOnResource mutex" << std::endl;
    mut.lock();
    std::this_thread::sleep_for(std::chrono::milliseconds(2000));
    mut.unlock();
}
void mutexLockSleep(){
    std::thread t1(workOnResourcem);
    std::thread t2(workOnResourcem);

    t1.join();
    t2.join();
}

// ----------------------------------------------
void ping5(){
    while(counter <= countlimit){
        signal2Ping.acquire();
        ++counter;
        signal2Pong.release();
    }
}
void pong5(){
    while(counter < countlimit){
        //signal2Pong.acquire();
        signal2Ping.release();
        signal2Pong.acquire();
    }
}
void pingPongSemaphore(){

    auto start = std::chrono::system_clock::now();
    signal2Ping.release();

    std::thread t1(ping5);
    std::thread t2(pong5);

    t1.join();
    t2.join();

    std::chrono::duration<double> dur = std::chrono::system_clock::now() - start;
    std::cout << "Duration: " << dur.count() << " seconds" << '\n';
}

//-----------------------------------------------
void ping4(){
    while(counter <= countlimit){
        atomicBool.wait(true);
        atomicBool.store(true);
        ++counter;
        atomicBool.notify_one();
    }
}
void pong4(){
    while(counter < countlimit){
        atomicBool.wait(false);
        atomicBool.store(false);
        atomicBool.notify_one();
    }
}
void pingPongAtomicBool(){
    std::cout << std::boolalpha << '\n';
    std::cout << "atomicBool.is_lock_free(): " << atomicBool.is_lock_free() << '\n';
    std::cout << '\n';

    auto start = std::chrono::system_clock::now();
    atomicBool.store(true);

    std::thread t1(ping4);
    std::thread t2(pong4);

    t1.join();
    t2.join();

    std::chrono::duration<double> dur = std::chrono::system_clock::now() - start;
    std::cout << "Duration: " << dur.count() << " seconds" << '\n';
}
//-----------------------------------------------
void ping3(){
    while(counter <= countlimit){
        condAtomicFlag1.wait(true);
        condAtomicFlag1.test_and_set();
        ++counter;
        condAtomicFlag1.notify_one();
    }
}
void pong3(){
    while(counter < countlimit){
        condAtomicFlag1.wait(false);
        condAtomicFlag1.clear();
        
        condAtomicFlag1.notify_one();
    }
}
void pingPongAtomicFlag(){
    auto start = std::chrono::system_clock::now();
    condAtomicFlag1.test_and_set();

    std::thread t1(ping3);
    std::thread t2(pong3);

    t1.join();
    t2.join();

    std::chrono::duration<double> dur = std::chrono::system_clock::now() - start;
    std::cout << "Duration: " << dur.count() << " seconds" << '\n';
}
// -------------------------------------------
void ping2(){
    while(counter <= countlimit){
        condAtomicFlag1.wait(false);
        condAtomicFlag1.clear();
        ++counter;
        condAtomicFlag2.test_and_set();
        condAtomicFlag2.notify_one();
    }
}
void pong2(){
    while(counter < countlimit){
        condAtomicFlag2.wait(false);
        condAtomicFlag2.clear();
        
        condAtomicFlag1.test_and_set();
        condAtomicFlag1.notify_one();
    }
}
void pingPongAtomicFlags(){
    auto start = std::chrono::system_clock::now();
    condAtomicFlag1.test_and_set();

    std::thread t1(ping2);
    std::thread t2(pong2);

    t1.join();
    t2.join();

    std::chrono::duration<double> dur = std::chrono::system_clock::now() - start;
    std::cout << "Duration: " << dur.count() << " seconds" << '\n';
}
// -------------------------------------
void ping(){
    while(counter <= countlimit){
        {
            std::unique_lock<std::mutex> lck(mutex_);
            // ожидает уведомления, удерживая уникальный замок
            // wait повторно захватывает mutex в момент пробуждения, когда через условную переменную приходит оповещение
            //предикат проверяется при захваченном мьютексе
            // если проверка проходит, то wait завершается, mutex остаётся захваченным и выполнение потока получателя продолжается
            // иначе wait освобождает mutex и поток уходит в ожидание
            // доп.предикат - защита от мнимого пробуждения
            // condVar1.wait(lck, []{ return dataReady; });
            // доп предикат - защита от утерянного пробуждения
            condVar1.wait(lck, []{ return dataReady == false; });
            dataReady =true;
        }
        ++counter;
        condVar2.notify_one();
    }
}
void pong(){
    while(counter < countlimit){
        {
            std::unique_lock<std::mutex> lck(mutex_);
            //std::lock_guard<std::mutex> lck(mutex_);
            condVar2.wait(lck, []{ return dataReady == true; });
            dataReady = false;
        }
        condVar1.notify_one();
    }
}
void pingPongConditionVar(){
    auto start = std::chrono::system_clock::now();

    std::thread t1(ping);
    std::thread t2(pong);

    t1.join();
    t2.join();

    std::chrono::duration<double> dur = std::chrono::system_clock::now() - start;
    std::cout << "Duration: " << dur.count() << " seconds" << '\n';
}
//---------------------------------------------------
void doTheWork(){
    std::cout << "Processing shared data" << std::endl;
}
void waitingForWork(){
    std::cout << "Worker: Waiting for work" << std::endl;
    std::unique_lock<std::mutex> lck(mutex_);
    condVar1.wait(lck,[]{ return dataReady; });
    doTheWork();
    std::cout << "Work done" << std::endl;
}
void setDataReady(){
    {
        std::lock_guard<std::mutex> lck(mutex_);
        dataReady = true;
    }
    std::cout << "Sender: Data is ready" << std::endl;
    condVar1.notify_one();
}
void conditionVariable(){
    std::cout << '\n';
    std::thread t1(waitingForWork);
    std::thread t2(setDataReady);
    t1.join();
    t2.join();
    std::cout << '\n';
}
// ==================================================
void prepareWork(){
    myVec.insert(myVec.end(), {0,1,0,3,8,12});
    std::cout << "Sender: Data prepared." << '\n';
    condAtomicFlag1.test_and_set();
    condAtomicFlag1.notify_one();
}
void completeWork(){
    std::cout << "Waiter: Waiting for data." << '\n';
    condAtomicFlag1.wait(false);
    myVec[2] = 2;
    std::cout << "Waiter: Complete the work." << '\n';
    for(auto i: myVec) std::cout << i << " ";
    std::cout << '\n';
}
void threadSynchronization(){
    std::cout << '\n';
    std::thread t1(prepareWork);
    std::thread t2(completeWork);
    t1.join();
    t2.join();
    std::cout << '\n';
}
//-------------------------------------
void prepareWorkSem(){
    myVec.insert(myVec.end(), {0,1,0,3,8,12});
    std::cout << "Sender: Data prepared." << '\n';
    signal2Ping.release();// +1 и разблокирует вызов acquire
    //condAtomicFlag1.test_and_set();
    //condAtomicFlag1.notify_one();
}
void completeWorkSem(){
    std::cout << "Waiter: Waiting for data." << '\n';
    signal2Ping.acquire();// изначально заблокировано т.к. signal2Ping(0)
    //condAtomicFlag1.wait(false);
    myVec[2] = 2;
    std::cout << "Waiter: Complete the work." << '\n';
    for(auto i: myVec) std::cout << i << " ";
    std::cout << '\n';
}

void threadSynchronizationSem(){
    std::cout << '\n';
    std::thread t1(prepareWorkSem);
    std::thread t2(completeWorkSem);
    t1.join();
    t2.join();
    std::cout << '\n';
}
// =====================================================
int main(int argc, char* argv[]){
    std::cout << "Case studies ..." << std::endl;
    //std::cout << "Arguments: " << argc << std::endl;
    //for(size_t i{}; i < argc; i++){
    //    std::cout << i << "-" << argv[i] << '\n';
    //}
    char sel = '1';
    if(argc > 1) {
        sel = argv[1][0];
    }
    switch (sel)
    {
        case '0':
        std::cout << "conditionVariable" << std::endl;
        for(int i = 0; i <10; i++) conditionVariable();
        break;
        case '1':
        std::cout << "pingPongConditionVar" << std::endl;
        pingPongConditionVar();
        break;
        case '2':
        std::cout << "pingPongAtomicFlags" << std::endl;
        pingPongAtomicFlags();
        break;
        case '3':
        std::cout << "pingPongAtomicFlag" << std::endl;
        pingPongAtomicFlag();
        break;
        case '4':
        std::cout << "pingPongAtomicBool" << std::endl;
        pingPongAtomicBool();
        break;
        case '5':
        std::cout << "pingPongSemaphore" << std::endl;
        pingPongSemaphore();
        break;
        case '6':
        std::cout << "threadSynchronization" << std::endl;
        threadSynchronization();
        break;
        case '7':
        std::cout << "threadSynchronizationSem" << std::endl;
        threadSynchronizationSem();
        break;
        case 'c':
        spinLockSleep();
        break;
        case 'm':
        mutexLockSleep();
        break;
        default:
        std::cout << "default sel: " << sel << '\n';
        std::cout << "Default => Empty" << std::endl;
        break;
    }
    return 0;
}