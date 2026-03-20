#include <iostream>

template <typename T>
T increment(T x) { return x + 1; }

template <typename T> class ArrayOf2{
    public:
    T& operator[] (size_t i) { return a_[i]; }
    const T& operator[] (size_t i) const { return a_[i]; }
    T sum() const { return a_[0] + a_[1]; }
    private:
    T a_[2];
};

template <typename T> constexpr T pi = T(3.14159265358979323846264338327950288419716939937510582097494459230781L);

int main(){
    std::cout << "Templates in C++" << std::endl;
    std::cout << "increment(4.2) = " << increment(4.2) <<std::endl;
    ArrayOf2<int> i; i[0] = 1; i[1] = 5;
    std::cout << "i.sum() = " << i.sum() <<std::endl;
    std::cout << "pi<float> = " << pi<float> <<std::endl;
    std::cout << "pi<double> = " << pi<double> <<std::endl;
    return 0;
}