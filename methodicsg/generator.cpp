#include <iostream>
#include <generator>
#include <ranges>

bool isPrime(int i){
    for(int j=2; j*j <= i; ++j){
        if(i % j == 0) return false;
    }
    return true;
}

std::generator<int> fib(){
    co_yield 0;
    auto a = 0;
    auto b = 1;
    for(auto n : std::views::iota(0)){
        auto next = a + b;
        a = b;
        b = next;
        co_yield next;
    }
}

void generat(){
    for(auto f : fib() | std::views::take(20)){
        std::cout << f << " ";
    }
    std::cout << '\n';
}

void rangeLazy(){
    std::cout << "Numbers from 1000000 to 1001000(displayed each 100th): " << '\n';
    for(int i: std::views::iota(1000000, 1001000)){
        if(i % 100 == 0) std::cout << i << " ";
    }
    std::cout << "\n";
    auto odd = [](int i){ return i % 2 == 1; };
    std::cout << "Odd numbers from 1000000 to 1001000(displayed each 100th): " << '\n';
    for(int i: std::views::iota(1000000, 1001000) | std::views::filter(odd)){
        if(i % 100 == 1) std::cout << i << " ";
    }
    std::cout << "\n";
    std::cout << "Prime numbers from 1000000 to 1001000: " << '\n';
    for(int i: std::views::iota(1000000, 1001000) | std::views::filter(odd) | std::views::filter(isPrime)){
        std::cout << i << " ";
    }
    std::cout << "\n";
    std::cout << "20 prime numbers starting with 1000000: " << '\n';
    for(int i: std::views::iota(1000000) | std::views::filter(odd) | std::views::filter(isPrime) | std::views::take(20)) {
        std::cout << i << " ";
    }
    std::cout << "\n";
}

int main(){
    generat();
    rangeLazy();
    return 0;
}