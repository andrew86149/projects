#include <iostream>
#include <atomic>
#include <random>
#include <thread>
#include <vector>

// дорого копировать
struct ExpensiveToCopy{
    int counter{};
};
int getRandom(int begin, int end){
    std::random_device seed;
    std::mt19937 engine(seed());
    std::uniform_int_distribution<> uniformDist(begin, end);

    return uniformDist(engine);
}
void count(ExpensiveToCopy& exp){
    std::vector<std::thread> v;
    //std::atomic<int> counter{exp.counter};
    std::atomic_ref<int> counter{exp.counter};

    for(int n=0; n<10; ++n){
        v.emplace_back([&counter]{
            auto randomNumber = getRandom(100, 200);
            for(int i=0; i<randomNumber; ++i) { ++counter; }
        });
    }
    for(auto& t : v) t.join();
}

void problem(){
    int val{5};
    int& ref = val;
    std::atomic<int> atomicRef(ref);
    ++atomicRef;
    std::cout << "ref: " << ref << '\n';
    std::cout << "atomicRef.load(): " << atomicRef.load() << '\n';
}

int main(){
    std::cout << "Atomic examples..." << std::endl;
    ExpensiveToCopy exp;
    count(exp);
    std::cout << "exp.counter: " << exp.counter << '\n';
    problem();
    return 0;
}