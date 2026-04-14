#include "main.h"

int main(int argc, char* argv[]){
    std::map<std::string, int> dc{
        {"one", 1},
        {"two", 2},
        {"three", 3},
        {"five", 5},
        {"six", 6},
        {"seven", 7},
        {"eight", 8},
        {"nine", 9},
        {"ten", 10}
    };
    std::cout << "methodics from Grimm..." << std::endl;
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
        runBench();
        //run();
        break;
        case 2:
        std::cout << "Аргумент #2...two\n";
        //spinlockrun();
        break;
        case 3:
        std::cout << "Аргумент #3...three\n";
        //mutrun();
        break;
        case 5:
        std::cout << "Аргумент #5...five\n";
        //threadSynchronisationAtomicFlag();
        break;
        case 6:
        std::cout << "Аргумент #6...six\n";
        //conditionVariableRun();
        break;
        case 7:
        std::cout << "Аргумент #7...seven\n";
        //atomicConditionRun();
        break;
        case 8:
        std::cout << "Аргумент #8...eight\n";
        //threadSynchronisationAtomicBool();
        break;
        case 9:
        std::cout << "Аргумент #9...nine\n";
        //atomicSharedPtr();
        //atomicSharedPtr();
        //atomicSharedPtr();
        //atomicSharedPtr();
        //atomicSharedPtr();
        break;
        case 10:
        std::cout << "Аргумент #10...ten\n";
        //producerConsumer();
        //transitivity();
        break;
    default:
    std::cout << "Аргумент всё остальное... " << sel << std::endl;
        break;
    }
    return 0;
}
