#include <iostream>
#include <concepts>
#include <type_traits>

template <typename T>
concept Integral = std::is_integral<T>::value;
template <typename T>
concept SignedIntegral = Integral<T> && std::is_signed<T>::value;
//template <typename T>
//concept UnsignedIntegral = Integral<T> && !SignedIntegral<T>;
template <typename T>
concept UnsignedIntegral = Integral<T> && 
requires(T){
    requires !SignedIntegral<T>;
};

void func(SignedIntegral auto integ){
    std::cout << "SignedIntegral: " << integ << '\n';
}

void func(UnsignedIntegral auto integ){
    std::cout << "UnsignedIntegral: " << integ << '\n';
}

template <typename T>
requires std::integral<T>
auto gcd(T a, T b){
    if(b==0) return a;
    else return gcd(b, a%b);
}

template <typename T>
auto gcd1(T a, T b) requires std::integral<T> {
    if(b==0) return a;
    else return gcd1(b, a%b);
}

template <std::integral T>
auto gcd2(T a, T b) {
    if(b==0) return a;
    else return gcd2(b, a%b);
}

auto gcd3(std::integral auto a, std::integral auto b) {
    if(b==0) return a;
    else return gcd3(b, a%b);
}

template <unsigned int i>
requires (i <= 20)
int sum(int j){ return i + j; }

struct Test{};


int main(){
    std::cout << "Concept example ...\n";
    std::cout << "gcd(100,12)= " << gcd(100,12) << std::endl;
    std::cout << "gcd1(100,12)= " << gcd1(100,12) << std::endl;
    std::cout << "gcd2(100,12)= " << gcd2(100,12) << std::endl;
    std::cout << "gcd3(100,12)= " << gcd3(100,12) << std::endl;
    std::cout << "sum<20>(2000): " << sum<20>(2000) << std::endl;
    //std::cout << "sum<23>(2000): " << sum<23>(2000) << std::endl; the expression ‘i <= 20 [with i = 23]’ evaluated to ‘false’
    std::cout << std::boolalpha;
    std::cout << "std::three_way_comparable<int>: " << std::three_way_comparable<int> << std::endl;
    std::cout << "std::three_way_comparable<double>: " << std::three_way_comparable<double> << std::endl;
    static_assert(std::three_way_comparable<std::string>);
    std::cout << "std::three_way_comparable<Test>: ";
    if constexpr(std::three_way_comparable<Test>) std::cout << "True";
    else std::cout << "False";
    std::cout << '\n';
    func(-5);
    func(5u);
    return 0;
}