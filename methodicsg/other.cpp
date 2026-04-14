#include <atomic>
#include <thread>
#include <chrono>
#include <mutex>
#include <vector>
#include <condition_variable>
#include <memory>


std::vector<int> mySharedWork;
std::string work;
std::atomic<bool> ready(false);
void consumer(){
    while(!ready.load()){}
    std::cout << work << std::endl;
}
void producer(){
    work = "done";
    ready = true;
}
void producerConsumer(){
    std::cout << "Последовательно-согласованное выполнение" << std::endl;
    std::thread prod(producer);
    std::thread con(consumer);
    prod.join();
    con.join();
}
std::atomic<bool> dataProduced(false);
std::atomic<bool> dataConsumed(false);
void dataProducer(){
    mySharedWork = {1,0,3};
    dataProduced.store(true, std::memory_order_release);
}
void deliveryBoy(){
    while(!dataProduced.load(std::memory_order_acquire));
    dataConsumed.store(true, std::memory_order_release);
}
void dataConsumer(){
    //while(!dataConsumed.load(std::memory_order_acquire));
    //dataProduced.load(std::memory_order_acquire);
    while(!dataProduced.load(std::memory_order_acquire));
    mySharedWork[1] = 2;
}
void transitivity(){
    std::cout << "Transitivity" << std::endl;
    std::cout << std::endl;
    std::thread t1(dataConsumer);
    //std::thread t2(deliveryBoy);
    std::thread t3(dataProducer);
    t1.join();
    //t2.join();
    t3.join();
    for(auto v: mySharedWork){
        std::cout << v << " ";
    }
    std::cout << "\n\n";
}
//------------------------------
void atomicSharedPtr(){
    std::cout << '\n';
    std::atomic<std::shared_ptr<std::string>> sharString(std::make_shared<std::string>("Zero"));
    std::thread t1([&sharString]{
        sharString.store(std::make_shared<std::string>(*sharString.load() + "One"));
    });
    std::thread t2([&sharString]{
        sharString.store(std::make_shared<std::string>(*sharString.load() + "Two"));
    });
    std::thread t3([&sharString]{
        sharString.store(std::make_shared<std::string>(*sharString.load() + "Three"));
    });
    std::thread t4([&sharString]{
        sharString.store(std::make_shared<std::string>(*sharString.load() + "Four"));
    });
    std::thread t5([&sharString]{
        sharString.store(std::make_shared<std::string>(*sharString.load() + "Five"));
    });
    t1.join();
    t2.join();
    t3.join();
    t4.join();
    t5.join();
    std::cout << *sharString.load() << '\n';
}

//-----------------------------------------
std::mutex mut;
std::mutex mutex_;

std::atomic<bool> dataReadyA(false);

void waitingForWorkA(){
    std::cout << "Waiting " << std::endl;
    while(!dataReadyA.load()){
        std::this_thread::sleep_for(std::chrono::milliseconds(5));
    }
    mySharedWork[1] = 2;
    std::cout << "Work done " << std::endl;
}
void setDataReadyA(){
    mySharedWork = {1,0,3};
    dataReadyA = true;
    std::cout << "Data prepared " << std::endl;
}
void atomicConditionRun(){
    std::cout << '\n';
    std::thread t1(waitingForWorkA);
    std::thread t2(setDataReadyA);
    t1.join();
    t2.join();
    for(auto v: mySharedWork){
        std::cout << v << " ";
    }
    std::cout << "\n\n";
}
// ---------------------------------------
std::condition_variable condVar;
bool dataReady{false};

void waitingForWork(){
    std::cout << "Waiting " << std::endl;
    std::unique_lock<std::mutex> lck(mutex_);
    condVar.wait(lck, []{ return dataReady; });
    mySharedWork[1] = 2;
    std::cout << "Work done " << std::endl;
}
void setDataReady(){
    mySharedWork = {1,0,3};
    {
        std::lock_guard<std::mutex> lck(mutex_);
        dataReady = true;
    }
    std::cout << "Data prepared " << std::endl;
    condVar.notify_one();
}
void conditionVariableRun(){
    std::cout << '\n';
    std::thread t1(waitingForWork);
    std::thread t2(setDataReady);
    t1.join();
    t2.join();
    for(auto v: mySharedWork){
        std::cout << v << " ";
    }
    std::cout << "\n\n";
}

//----------------------------------
std::vector<int> myVec{};
std::atomic_flag atomicFlag{};
std::atomic<bool> atomicBool{false};

void prepareWorkB(){
    myVec.insert(myVec.end(), {0,1,0,3});
    std::cout << "Sender: Data prepared." << '\n';
    //atomicFlag.test_and_set();
    atomicBool.store(true);
    //atomicFlag.notify_one();
    atomicBool.notify_one();
}
void completeWorkB(){
    std::cout << "Waiter: Waiting for data." << '\n';
    atomicBool.wait(false);
    myVec[2] = 2;
    std::cout << "Waiter: Complete the work." << '\n';
    for(auto i: myVec) std::cout << i << " ";
    std::cout << '\n';
}

void threadSynchronisationAtomicBool(){
    std::cout << '\n';
    std::thread t1(prepareWorkB);
    std::thread t2(completeWorkB);
    t1.join();
    t2.join();
    std::cout << '\n';
}
//----------------------------------
void prepareWork(){
    myVec.insert(myVec.end(), {0,1,0,3});
    std::cout << "Sender: Data prepared." << '\n';
    atomicFlag.test_and_set();
    atomicFlag.notify_one();
}
void completeWork(){
    std::cout << "Waiter: Waiting for data." << '\n';
    atomicFlag.wait(false);
    myVec[2] = 2;
    std::cout << "Waiter: Complete the work." << '\n';
    for(auto i: myVec) std::cout << i << " ";
    std::cout << '\n';
}

void threadSynchronisationAtomicFlag(){
    std::cout << '\n';
    std::thread t1(prepareWork);
    std::thread t2(completeWork);
    t1.join();
    t2.join();
    std::cout << '\n';
}
// ----------------------------------

void mworkOnResource(){
    mut.lock();
    std::cout << "mworkOnResource" << std::endl;
    std::this_thread::sleep_for(std::chrono::milliseconds(4000));
    mut.unlock();
}
void mutrun(){
    std::thread t1(mworkOnResource);
    std::thread t2(mworkOnResource);
    t1.join();
    t2.join();
}

class Spinlock{
    std::atomic_flag flag = ATOMIC_FLAG_INIT;
    public:
    void lock(){ while(flag.test_and_set()); }
    void unlock(){ flag.clear(); }
};
Spinlock spin;
void workOnResource(){
    spin.lock();
    std::cout << "workOnResource" << std::endl;
    std::this_thread::sleep_for(std::chrono::milliseconds(4000));
    spin.unlock();
}
void spinlockrun(){
    std::thread t1(workOnResource);
    std::thread t2(workOnResource);
    t1.join();
    t2.join();
}
// -----------------
struct Example{
    char a;
    int b : 5;
    int c : 11,
          : 0,
        d : 8;
    int e;
    double f;
    std::string g;
} obj;

void run(){
    obj.b = 31;
    std::cout << "obj.b = " << obj.b << std::endl;
    //obj.c = 2;
    std::cout << "obj.c = " << obj.c << std::endl;
    //obj.d = 3;
    std::cout << "obj.c = " << obj.d << std::endl;
}