#include <any>
#include <optional>
#include <vector>
#include <string>
#include <iostream>
#include <variant>
#include <expected>

class MyClass{};

void anyex(){
    std::vector<std::any> anyVec{ true, 2017, std::string("test"), 3.14, MyClass() };
    std::cout << std::any_cast<bool>(anyVec[0]) << std::endl;
    std::cout << std::any_cast<int>(anyVec[1]) << std::endl;
    std::cout << anyVec[0].type().name() << std::endl;// строковое представление tipeid
    std::cout << anyVec[1].type().name() << std::endl;
    std::cout << anyVec[2].type().name() << std::endl;
    std::cout << anyVec[3].type().name() << std::endl;
    std::cout << anyVec[4].type().name() << std::endl;
    std::cout << "------------------------" << std::endl;
}

std::optional<int> getFirst(const std::vector<int>& vec){
    if(!vec.empty()) return std::optional<int>(vec[0]);
    else return std::optional<int>();
}
std::optional<int> getInt(std::string arg){
    try{
        return {std::stoi(arg)};
    } catch(...){
        return {};
    }
}
void optionalex(){
    std::vector myVec{10,2,3};
    std::vector<int> myEmptyVec;
    auto myInt = getFirst(myVec);
    if(myInt){
        std::cout << *myInt << std::endl;
        std::cout << myInt.value() << std::endl;
        std::cout << myInt.value_or(2017) << std::endl;
    }
    auto myEmptyInt = getFirst(myEmptyVec);
    if(!myEmptyInt){
        std::cout << myEmptyInt.value_or(2017) << std::endl;
    }
    std::vector<std::optional<std::string>> strings = {"66", "foo", "-5"};
    // -std=c++23
    for(auto s: strings){
        auto res = s.and_then(getInt)
        .transform([](int n){ return n + 100; })
        .transform([](int n){ return std::to_string(n); })
        .or_else([] {return std::optional{std::string("Error")};});
        std::cout << *res << ' ';
    }
    std::cout << std::endl;
    std::cout << "------------------------" << std::endl;
}

void variantex(){
    std::variant<int,float> v,w;
    v = 12;
    int i = std::get<int>(v);
    w = std::get<int>(v);
    w = std::get<0>(v);
    w = v;
    try{
        std::get<float>(w);
    } catch(std::bad_variant_access&){}
    std::vector<std::variant<char,long,float,int,double,long long>>
    vecVariant = {5,'2',5.4,100ll,2011l,3.5f,2017};
    for(auto& v : vecVariant){
        std::visit([](auto&& arg){ std::cout << arg << " ";}, v);
    }
    std::cout << std::endl;
    for(auto& v : vecVariant){
        std::visit([](auto&& arg){ std::cout << typeid(arg).name() << " ";}, v);
    }
    std::cout << std::endl;
    // получить сумму
    std::common_type<char,long,float,int,double,long long>::type res{};
    std::cout << typeid(res).name() << '\n';
    for(auto& v : vecVariant){
        std::visit([&res](auto&& arg){ res += arg;}, v);
    }
    std::cout << "res: " << res << '\n';
    for(auto& v : vecVariant){
        std::visit([&res](auto&& arg){ arg *= 2;}, v);
        std::visit([](auto&& arg){ std::cout << arg << " ";}, v);
    }
    std::cout << std::endl;
}

std::expected<int, std::string> getIntEx(std::string arg){
    try{
        return {std::stoi(arg)};
    } catch(...){
        return std::unexpected{std::string(arg + ": Error")};
    }
}

void expectedex(){
    std::vector<std::string> strings = {"66", "foo", "-5"};
    for(auto s: strings){
        auto res = getIntEx(s);
        if(res){
            std::cout << res.value() << ' ';
        } else {
            std::cout << res.error() << ' ';
        }
    }
    std::cout << '\n';
    for(auto s: strings){
        auto res = getIntEx(s);
        std::cout << res.value_or(2023) << ' ';
    }
    std::cout << '\n';
    // монадические операции
    for(auto s: strings){
        auto res = getIntEx(s).transform([](int n){ return n + 100; })
                              .transform([](int n){ return std::to_string(n); });
        std::cout << *res << ' ';
    }
    std::cout << '\n';
}

int main(){
    anyex();
    optionalex();
    variantex();
    expectedex();
    return 0;
}