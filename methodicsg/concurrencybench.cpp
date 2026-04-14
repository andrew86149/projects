#include <chrono>
#include <iostream>
#include <random>
#include <vector>
#include <mutex>
#include <atomic>

constexpr long long size = 100'000'000;
std::mutex mmutex;

void runBench(){
    std::cout << "runBench" << std::endl;
    std::vector<int> randValues;
    randValues.reserve(size);
    //random values
    std::random_device seed;
    std::mt19937 engine(seed());
    std::uniform_int_distribution<> uniformDist(1,10);
    const auto ldm = std::chrono::steady_clock::now();
    for(long long i = 0; i < size; ++i){
        randValues.push_back(uniformDist(engine));
    }
    const auto sta = std::chrono::steady_clock::now();
    //unsigned long long sum = {};
    std::atomic<unsigned long long> sum = {};
    std::cout << std::boolalpha << "sum.is_lock_free():" << sum.is_lock_free() << std::endl;
    //for(auto n : randValues) sum += n;
    for(auto n : randValues) {
        //std::lock_guard<std::mutex> mlg(mmutex);
        //sum += n;
        sum.fetch_add(n);
    }
    //const unsigned long long sum = std::accumulate(randValues.begin(),randValues.end(),0);
    //const unsigned long long sum = std::accumulate(randValues.cbegin(),randValues.cend(),0);
    const std::chrono::duration<double> dur = std::chrono::steady_clock::now() - sta;
    std::cout << "Время сложения " << dur.count() << " секунд" << std::endl;
    const std::chrono::duration<double> dur2 = std::chrono::steady_clock::now() - ldm;
    std::cout << "Время заполнения массива " << dur2.count() << " секунд" << std::endl;
    std::cout << "Result: " << sum << std::endl;
}