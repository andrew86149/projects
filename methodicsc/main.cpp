#include "count_lines.h"

int main(int argc, char *argv[]){
    std::map<std::string, int> dc{
        {"one", 1},
        {"two", 2},
        {"three", 3},
        {"funr", 5},
        {"mond", 6}
    };
    std::cout << "Functional examples ..." << std::endl;
    int sel = 0;
    if(argc < 2){
        std::cout << "Запущено без аргументов\n";
    } else {
        sel = dc[argv[1]];
    }
    switch (sel)
    {
        case 0:
        std::cout << "Нет такой записи в карте, т.е. 0...\n";
        break;
        case 1:
        std::cout << "Аргумент #1...one\n";
        run();
        break;
        case 2:
        std::cout << "Аргумент #2...two\n";
        serialex();
        break;
        case 3:
        std::cout << "Аргумент #3...three\n";
        greaterrun();
        break;
        case 5:
        std::cout << "Аргумент #5...funr\n";
        funr();
        break;
        case 6:
        std::cout << "Аргумент #6...mond\n";
        mond();
        break;
    default:
    std::cout << "Аргумент всё остальное... " << sel << std::endl;
        break;
    }
    return 0;
}