#include <iostream>
#include <vector>
#include <ranges>
#include <string>

template <typename Func, typename Seq>
auto filter(Func func, const Seq& seq){
    typedef typename Seq::value_type value_type;
    std::vector<value_type> result{};
    for(auto i : seq | std::views::filter(func)) result.push_back(i);
    return result;
}

int main(){
    std::cout << "Examples..." << std::endl;
    std::vector<std::string> myStrings{"ГравиЛинк","АСКОН","АЛМИ Партнер","МКО Системы","DevSphere",
        "Ведал","Аквариус.R&D","Бюро 1440","Пространственные системы информации","РегЛаб","FutbiTex",
    "ИнфоТеКС","Форвард Энерго","НИКИРЭТ","ТРЭИ","ТнсЭнергоПенза","ГИТ","Рубин","Открытые решения",
"Код безопасности","CodeInside"};
    auto res2 = filter([](const std::string& s){ return std::isupper(s[0]); }, myStrings);
    std::cout << "\n\n";
    for(auto word: res2) std::cout << word << '\n';
    std::cout << '\n';
    return 0;
}