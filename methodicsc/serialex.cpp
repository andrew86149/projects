#include "count_lines.h"
#include "nlohmann/json.hpp"

using json = nlohmann::json;

void serialex(){
    std::cout << "serialex starting...\n";

    json j;
    j["pi"] = 3.141;
    j["happy"] = "Niels";
    j["nothing"] = nullptr;
    j["answer"]["everything"] = 42;
    j["list"] = {1,0,2};
    j["object"] = {{"currency", "USD"}, {"value", 42.99}};
    std::string s = j.dump();
    std::cout << j.dump(4) << std::endl;

    std::ofstream o("example.json");
    //o << std::setw(4) << j << std::endl;
    o << j << std::endl;

    std::ifstream f("example.json");
    //f >> j;
    json data = json::parse(f);
    std::cout << data << std::endl;
}