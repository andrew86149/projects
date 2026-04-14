#include "count_lines.h"

/*template <typename T, typename F>
auto mbind(const std::vector<T>& xs, F f){
    auto transformed = xs | view::transform(f) | to_vector;
    return transformed | view::join | to_vector;
}*/
// Универсальный механизм превращения бинарных функций в унарные
/*template <typename Function, typename SecondArgType>
class partial_application_on_2nd_impl{
    public:
    partial_application_on_2nd_impl(Function function, SecondArgType second_arg): m_function(function), m_second_arg(second_arg){}
    private:
    Function m_function;
    SecondArgType m_second_arg;
};

template <typename Function, typename SecondArgType>
class partial_application_bind2nd_impl{
    public:
    
    template <typename FirstArgType>
    auto operator()(FirstArgType&& first_arg)const -> decltype(m_function(std::forward<FirstArgType>(first_arg),m_second_arg)){
        return m_function(std::forward<FirstArgType>(first_arg),m_second_arg);
    }
};

template <typename Function, typename SecondArgType>
partial_application_bind2nd_impl<Function, SecondArgType>
bind2nd(Function&& function, SecondArgType&& second_arg){
    return partial_application_bind2nd_impl<Function, SecondArgType>(
        std::forward<Function>(function),
        std::forward<SecondArgType>(second_arg)
    );
}*/

class greater_than{
    public:
    greater_than(int value): m_value(value){}
    bool operator()(int arg)const{return arg>m_value;}
    private:
    int m_value;
};

bool greater(double first, double second){ return first > second; }
auto greater_curried(double first){ 
    return [first](double second){
        return first > second;
    }; 
}
//Функции подъёма
template <typename Function>
auto pointer_lift(Function f){
    return [f](auto* item){
        if(item) {
            f(*item);
        }
    };
}
template <typename Function>
auto collection_lift(Function f){
    return [f](auto& items){
        for(auto& item : items) {
            f(item);
        }
    };
}
std::vector<unsigned int> cache{0,1};
unsigned int fib(unsigned int n){
    if(cache.size() > n){
        return cache[n];
    } else {
        const auto result = fib(n - 1) + fib(n - 2);
        cache.push_back(result);
        return result;
    }
}


void greaterrun(){
    std::cout << "greaterrun" << std::endl;
    std::cout << std::boolalpha << '\n';
    greater_than greater_than_42(42);
    std::cout << "greater_than_42(1) " << greater_than_42(1) << std::endl;
    std::cout << "greater_than_42(50) " << greater_than_42(50) << std::endl;

    //auto greater_than_43 = bind2nd(std::greater<int>(), 43);
    //std::cout << "greater_than_43(1) " << greater_than_43(1) << std::endl;
    //std::cout << "greater_than_43(50) " << greater_than_43(50) << std::endl;
    std::cout << "greater(2,3) = " << greater(2,3) << std::endl;
    //std::cout << "greater_curried(2) = " << greater_curried(2) << std::endl;
    std::cout << "greater_curried(2)(3) = " << greater_curried(2)(3) << std::endl;
    for(int i = 1; i < 20; i++)
    std::cout << fib(i) << " ";
    std::cout << '\n';
}