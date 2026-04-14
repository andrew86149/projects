#include <iostream>
#include <chrono>
#include <thread>
//#include <mutex>
#include <syncstream>
#include <vector>
#include <semaphore>
#include <map>
//std::mutex coutMutex;
#include <atomic>

std::vector<int> mySharedWork;
std::atomic<bool> dataReady(false);

void waitingForWork(){
    std::cout << "Waiting " << '\n';
    while ( !dataReady.load() ){
        std::this_thread::sleep_for(std::chrono::milliseconds(5));
    }
    mySharedWork[1] = 25;
    std::cout << "Work done " << '\n';
}
void setDataReady(){
    mySharedWork = {1, 0, 3};
    dataReady = true;
    std::cout << "Data prepared" << '\n';
}
void atomicCondition(){
    std::cout << '\n';

  std::thread t1(waitingForWork);
  std::thread t2(setDataReady);

  t1.join();
  t2.join();
  
  for (auto v: mySharedWork){
      std::cout << v << " ";
  }
      
  
  std::cout << "\n\n";
}

class Worker{
    public:
    Worker(std::string n): name(n) {}
    void operator() (){
        for(int i = 1; i <= 3; ++i){
            std::this_thread::sleep_for(std::chrono::milliseconds(200));
            std::osyncstream syncStream(std::cout);
            syncStream << name << ": " << "Work " << i << " done !!!" << '\n';
            //std::lock_guard<std::mutex> coutLock(coutMutex);
            //std::cout << name << ": " << "Work " << i << " done !!!" << '\n';
        }
    }
    private:
    std::string name;
};

void coutSyn(){
    std::cout << '\n';
    std::cout << "Boss: Let's start working.\n\n";
    std::thread herb = std::thread(Worker("Herb"));
    std::thread andrei = std::thread(Worker("  Andrei"));
    std::thread scott = std::thread(Worker("    Scott"));
    std::thread bjarne = std::thread(Worker("      Bjarne"));
    std::thread bart = std::thread(Worker("        Bart"));
    std::thread jenne = std::thread(Worker("          Jenne"));

    herb.join();
    andrei.join();
    scott.join();
    bjarne.join();
    bart.join();
    jenne.join();

    std::cout << '\n' << "Boss: Let's go home." << '\n';
}

std::vector<int> myVec{};
std::counting_semaphore<1> prepareSignal(0);

void prepareWork(){
    myVec.insert(myVec.end(), {0,1,0,3});
    std::cout << "Sender: Data prepared." << '\n';
    prepareSignal.release();
}
void completeWork(){
    std::cout << "Waiter: Waiting for data." << '\n';
    prepareSignal.acquire();
    myVec[2] = 2;
    std::cout << "Waiter: Complete the work." << '\n';
    for(auto i: myVec) std::cout << i << " ";
    std::cout << '\n';
}

void threadSynchronizationSemaphore(){
    std::cout << '\n';
    std::thread t1(prepareWork);
    std::thread t2(completeWork);
    t1.join();
    t2.join();
    std::cout << '\n';
}

int main(int argc, char* argv[]){
    std::map<std::string, int> dc{
        {"one", 1},
        {"two", 2},
        {"three", 3},
        {"five", 5},
        {"vector", 6}
    };
    std::cout << "Concurrency examples ..." << std::endl;
    int sel = 0;
    if(argc < 2){
        std::cout << "Запущено без аргументов\n";
    } else {
        sel = dc[argv[1]];
    }
    switch (sel)
    {
        case 0:
        std::cout << "Нет такой записи в карте, т.е. 0...\n";
        break;
        case 1:
        std::cout << "Аргумент #1...one\n";
        coutSyn();
        break;
        case 2:
        std::cout << "Аргумент #2...two\n";
        threadSynchronizationSemaphore();
        break;
        case 3:
        std::cout << "Аргумент #3...three\n";
        atomicCondition();
        break;
    default:
    std::cout << "Аргумент всё остальное... " << sel << std::endl;
        break;
    }
    return 0;
}