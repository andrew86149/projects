#include <iostream>
#include <atomic>
#include <thread>

template <typename T>
T fetch_mult(std::atomic<T>& shared, T mult){
    T oldValue = shared.load();
    while(!shared.compare_exchange_strong(oldValue,oldValue * mult));
    return oldValue;
}

void fetch_mult_ex(){
    std::atomic<int> myInt{5};
    std::cout << myInt << '\n';
    fetch_mult(myInt,5);
    std::cout << myInt << '\n';
}

void helloFunction(){
    std::cout << "function" << std::endl;
    std::cout << "Hello C++11 from a function." << '\n';
}

class HelloFunctionObject{
    public:
    void operator()()const {
        std::cout << "function object" << std::endl;
        std::cout << "Hello C++11 from a function object." << '\n';
    }
};

void threadCreate(){
    std::cout << '\n';
    std::thread t1(helloFunction);
    HelloFunctionObject helloFunctionObject;
    std::thread t2(helloFunctionObject);
    std::thread t3([]{ 
        std::cout << "lambda function" << std::endl; 
        std::cout << "Hello C++11 from lambda function." << '\n';
    });
    t1.join();
    t2.join();
    t3.join();
    std::cout << '\n';
}

class Sleeper{
  public:
    Sleeper(int& i_):i{i_}{};
    void operator() (int k){
      for (unsigned int j= 0; j <= 5; ++j){
        std::this_thread::sleep_for(std::chrono::milliseconds(100));
        i += k;
      }
      std::cout << std::this_thread::get_id();// неопределённое поведение?
    }
  private:
    int& i;
};

void threadArguments(){
    std::cout << '\n';
    int valSleeper= 1000;
    std::thread t(Sleeper(valSleeper), 5);
    t.detach();
    std::cout << "valSleeper = " << valSleeper << '\n';// неопределённое поведение?
    std::cout << '\n';
}

int main(){
    std::cout << "Examples started..." << std::endl;
    fetch_mult_ex();
    threadCreate();
    threadArguments();
    return 0;
}