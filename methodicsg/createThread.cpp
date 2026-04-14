#include "main.h"

void helloFunction(){
    std::cout << "Hello from a function" << std::endl;
}
class HelloFunctionObject{
    public:
    void operator()() const {
        std::cout << "Hello from a function object" << std::endl;
    }
};

void createThread(){
    std::cout << std::endl;
    std::thread t1(helloFunction);
    
    HelloFunctionObject helloFunctionObject;
    std::thread t2(helloFunctionObject);

    std::thread t3([]{ std::cout << "Hello from a lambda" << std::endl; });

    t1.join();
    t2.join();
    t3.join();
    std::cout << "Done" << std::endl;
}

void threadWithoutJoin(){
    std::cout << std::boolalpha << std::endl;
    std::cout << "hardware_concurrency(): " << std::thread::hardware_concurrency() << std::endl;
    std::thread t([]{ std::cout << "thread get_id: " << std::this_thread::get_id() << std::endl; });
    t.join();
    std::string s("C++11");
    std::thread t1([=]{ std::cout << s << std::endl; });
    
    std::thread t2([&]{ std::cout << "by ref " << s << std::endl; });
    //t2.detach();// м.б.неопределённое поведение
    std::cout << "t1.joinable(): " << t1.joinable() << std::endl;
    t2.join();
    t1.join();
    std::cout << "t1.joinable(): " << t1.joinable() << std::endl;
    std::jthread t3([]{ std::cout << "thread get_id: " << std::this_thread::get_id() << std::endl; });
    std::cout << "t3.joinable(): " << t3.joinable() << std::endl;
}
struct CriticalData{
    std::mutex mut;
    int x;
};
void deadlockfunck(CriticalData& a, CriticalData& b){
    std::unique_lock<std::mutex> guard1(a.mut, std::defer_lock);
    std::cout << "deadlockfunc" << std::endl;
    std::cout << "Thread: " << std::this_thread::get_id() << " first mutex " << std::endl;
    //a.mut.lock();
    std::cout << " get the first mutex " << std::endl;
    std::this_thread::sleep_for(std::chrono::milliseconds(1));
    std::unique_lock<std::mutex> guard2(b.mut, std::defer_lock);
    std::cout << "Thread: " << std::this_thread::get_id() << " second mutex " << std::endl;
    std::cout << "Thread: " << std::this_thread::get_id() << " get both mutexes " << std::endl;
    //b.mut.lock();
    std::cout << " get the second mutex " << std::endl;
    std::lock(guard1,guard2);
    std::cout << "do something" << std::endl;
    //a.mut.unlock();
    //b.mut.unlock();
}
void deadlock(){
    CriticalData c1;
    CriticalData c2;

    std::thread t1([&]{ deadlockfunck(c1,c2); });
    std::thread t2([&]{ deadlockfunck(c2,c1); });

    t1.join();
    t2.join();
}
std::map<std::string,int> teleBook{
    {"Dijkstra", 1972}, {"Scott", 1976}, {"Ritchie", 1983}
};
std::shared_timed_mutex teleBookMutex;
void addToTeleBook(const std::string& na, int tele){
    std::lock_guard<std::shared_timed_mutex> writeLock(teleBookMutex);
    std::cout << "\nSTARTING UPDATE " << na;
    std::this_thread::sleep_for(std::chrono::milliseconds(500));
    teleBook[na] = tele;
    std::cout << "... ENDING UPDATE " << na << std::endl;
}
void printNumber(const std::string& na){
    std::shared_lock<std::shared_timed_mutex> readerLock(teleBookMutex);
    auto searchEntry = teleBook.find(na);
    if(searchEntry != teleBook.end()){
        std:: cout << searchEntry->first << ": " << searchEntry->second << std::endl;
    } else {
        std::cout << na << "not found!" << std::endl;
    }
    //std::cout << na << ": " << teleBook[na];
}
void readerWriterLock(){
    std::cout << std::endl;
    std::thread reader1([]{ printNumber("Scott"); });
    std::thread reader2([]{ printNumber("Ritchie"); });
    std::thread w1([]{ addToTeleBook("Scott",1968); });
    std::thread reader3([]{ printNumber("Dijkstra"); });
    std::thread reader4([]{ printNumber("Scott"); });
    std::thread w2([]{ addToTeleBook("Bjarne",1965); });
    std::thread reader5([]{ printNumber("Scott"); });
    std::thread reader6([]{ printNumber("Ritchie"); });
    std::thread reader7([]{ printNumber("Scott"); });
    std::thread reader8([]{ printNumber("Bjarne"); });
    reader1.join();
    reader2.join();
    reader3.join();
    reader4.join();
    reader5.join();
    reader6.join();
    reader7.join();
    reader8.join();
    w1.join();
    w2.join();
    std::cout << std::endl;
    std::cout << "\nThe new telephone book" << std::endl;
    for(auto teleIt : teleBook){
        std::cout << teleIt.first << ": " << teleIt.second << std::endl;
    }
    std::cout << std::endl;
}

std::mutex coutMutex;
thread_local std::string s("hello from ");

void addThreadLocal(std::string const& s2){
    s += s2;
    std::lock_guard<std::mutex> guard(coutMutex);
    std::cout << s << std::endl;
    std::cout << "&s: " << &s << std::endl;
    std::cout << std::endl;
}
void threadLocal(){
    std::cout << std::endl;
    std::thread t1(addThreadLocal, "t1");
    std::thread t2(addThreadLocal, "t2");
    std::thread t3(addThreadLocal, "t3");
    std::thread t4(addThreadLocal, "t4");
    t1.join();
    t2.join();
    t3.join();
    t4.join();
}
bool dataReady{false};
std::condition_variable condVar;

void doTheWork(){
    std::cout << "Processing shared data " << std::endl;
}

void waitingForWork(){
    std::cout << "Worker: Waiting for work" << std::endl;
    std::unique_lock<std::mutex> lck(coutMutex);
    //condVar.wait(lck);
    condVar.wait(lck, []{ return dataReady; });
    doTheWork();
    std::cout << "Work done" << std::endl;
}

void setDataReady(){
    {
        std::lock_guard<std::mutex> lck(coutMutex);
        dataReady = true;
    }
    std::cout << "Sender: Data is ready" << std::endl;
    condVar.notify_one();
}

void conditionVariableBlock(){
    std::cout << std::endl;
    std::thread t1(setDataReady);
    std::thread t2(waitingForWork);
    t1.join();
    t2.join();
    std::cout << std::endl;
}