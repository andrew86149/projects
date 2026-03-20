#include <iostream>
#include <string>
#include <vector>
#include <unordered_map>
#include <fstream>
#include <algorithm>
#include <iterator>


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
std::vector<int>
count_lines_in_files(const std::vector<std::string> &files)
{
    std::vector<int> results;

    for (const auto &file: files) {
        results.push_back(count_lines(file));
    }

    return results;
}

int main(int argc, char *argv[])
{
    auto results = count_lines_in_files({"CMakeCache.txt", "Makefile"});

    for (auto result: results) {
        std::cout << result << " line(s)\n";
    }

    return 0;
}