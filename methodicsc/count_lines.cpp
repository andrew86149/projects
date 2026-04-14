#include "count_lines.h"

int f(int previous_count, char c){
    return(c != '\n')? previous_count : previous_count + 1;
}
int count_linesv(const std::string& s){
    return std::accumulate(s.cbegin(), s.cend(),0,f);//Провести свертку по всей строке
}
int acount_lines(const std::string &filename)
{
    std::ifstream in(filename);

    // Мы создаем итератор для входного потока и передаем его в алгоритм count, 
    // чтобы подсчитать количество символов новой строки.
    return std::count(
        std::istreambuf_iterator<char>(in),
        std::istreambuf_iterator<char>(),
        '\n');
}

// Эта функция открывает файл, указанный в аргументе filename, и подсчитывает количество строк в нем.
int count_lines(const std::string &filename)
{
    std::ifstream in(filename);
    in.unsetf(std::ios_base::skipws);

    // Мы создаем итератор для входного потока и передаем его в алгоритм count, 
    // чтобы подсчитать количество символов новой строки.
    return std::count(
        std::istream_iterator<char>(in),
        std::istream_iterator<char>(),
        '\n');
}


// Для заданного списка файлов эта функция возвращает список с количеством строк в каждом из них.
std::vector<int> count_lines_in_files(const std::vector<std::string> &files)
{
    std::vector<int> results;

    for (const auto &file: files) {
        results.push_back(acount_lines(file));
    }

    return results;
}

std::vector<int> tcount_lines_in_files(const std::vector<std::string>& files){
    std::vector<int> results(files.size());
    std::transform(files.cbegin(), files.cend(),results.begin(),acount_lines);
    return results;
}

//std::vector<int> rcount_lines_in_files(const std::vector<std::string>& files){
    //return files | std::transform(count_lines);
    //return files | std::ranges::transform(acount_lines);
    //return files | transform(count_lines);
//}

// императивное решение
std::vector<int> icount_lines_in_files(const std::vector<std::string>& files){
    std::vector<int> results;
    char c = 0;

    for (const auto &file: files) {
        int line_count = 0;
        std::ifstream in(file);
        while(in.get(c)){     // нарушает требование чистоты
            if(c == '\n'){
                line_count++; // нарушает требование чистоты
            }
        }
        results.push_back(line_count); // нарушает требование чистоты
        //results.push_back(count_lines(file));
    }

    return results;
}
//------------------------------------------------------
double average_score(const std::vector<int>& scores){
    int sum = 0;
    for(int score : scores){
        sum += score;
    }
    return sum/(double)scores.size();
}
double average_scoref(const std::vector<int>& scores){
    //return std::accumulate(scores.cbegin(),scores.cend(),0)/(double)scores.size();
    return std::reduce(std::execution::par,scores.cbegin(),scores.cend(),0)/(double)scores.size();
}
double scores_product(const std::vector<int>& scores){
    return std::accumulate(scores.cbegin(),scores.cend(),1,std::multiplies<int>());
}
/*std::string trim_left(std::string s){
    s.erase(s.begin(), std::find_if(s.begin(), s.end(), is_not_space));
    return s;
}
std::string trim_right(std::string s){
    s.erase(std::find_if(s.rbegin(), s.rend(), is_not_space).base(), s.end());
    return s;
}
std::string trim(std::string s){
    return trim_left(trim_right(std::move()));
}*/
//------------------------------------------------------
auto factorial(int n){
    if(n==0) {
        return 1;
    } else {
        return factorial(n - 1) * n;
    }
}
template <typename Object, typename Function>
decltype(auto) call_on_object(Object&& object, Function function){
    return function(std::forward<Object>(object));
}
int ask() { return 42; }
typedef decltype(ask)* function_ptr;
class convertible_to_function_ptr{
    public:
    operator function_ptr() const{
        return ask;
    }
};
template <typename T>
class older_than{
    public:
    older_than(int limit): m_limit(limit){}
    bool operator()(const T& object) const{
        return object.age() > m_limit;
    }
    private:
    int m_limit;
};
class person{
    int m_age;
    public:
    person(int age): m_age(age){}
    int age(){ return m_age; }
};
//======================================================
void run(){
    auto iresults = icount_lines_in_files({"CMakeCache.txt", "Makefile"});
    std::cout << "Императивное решение\n";
    for (auto result: iresults) {
        std::cout << result << " line(s)\n";
    }
    std::cout << "Использование алгоритма std::count\n";
    auto results = count_lines_in_files({"CMakeCache.txt", "Makefile"});

    for (auto result: results) {
        std::cout << result << " line(s)\n";
    }

    std::cout << "Использование алгоритма std::transform\n";
    auto tresults = tcount_lines_in_files({"CMakeCache.txt", "Makefile"});

    for (auto result: tresults) {
        std::cout << result << " line(s)\n";
    }
    //auto rresults = rcount_lines_in_files({"CMakeCache.txt", "Makefile"});

    //for (auto result: rresults) {
    //    std::cout << result << " line(s)\n";
    //}
    std::vector scores{1,2,3,4,5,6,7,8,9,10};
    std::cout << "Среднее арифметическое " << average_scoref(scores) << std::endl;
    std::cout << "Произведение всех оценок " << scores_product(scores) << std::endl;
    std::cout << "Factorial 6 = " << factorial(6) << std::endl;
    auto ask_ptr = &ask;
    std::cout << "Указатель на функцию " << ask_ptr() << std::endl;
    auto ask_ref = ask;
    std::cout << "Ссылка на функцию " << ask_ref() << std::endl;
    convertible_to_function_ptr ask_wrapper;
    std::cout << "Объект,допускающий неявное преобразование в указатель на функцию " << ask_wrapper() << std::endl;
}