//
// Created by Yancey666 on 2023/11/5.
//
#include <iostream>
#include <fstream>
#include "main.h"
#include "core/lexer/Lexer.h"
#include "nlohmann/json.hpp"
#include "core/resources/CPack.h"


int main() {
//    testLexer("setblock minecraft:stone");
//    testLexer("give asd\"ffd apple 64\" 64");
//    testLexer("give @s[x=!64] minecraft:apple 64");
//    testLexer("tp +0.0.1 -0.0 ~0.0 .1");

//    std::ifstream f(R"(D:\CLion\project\CHelper\resources\manifest.json)");
//    nlohmann::json data = nlohmann::json::parse(f);
//    f.close();
//    Manifest manifest = data.get<Manifest>();
//    std::cout << manifest << std::endl;
    using namespace std;
    using namespace CHelper;
    using namespace std::filesystem;
    CPack cpack = CPack::getInstance(path(R"(D:\CLion\project\CHelper\resources)"));
//    std::ifstream f(R"(D:\CLion\project\CHelper\resources\manifest.json)");
//    nlohmann::json data = nlohmann::json::parse(f);
//    f.close();
//    Manifest manifest = data.get<Manifest>();
//    std::cout << manifest << std::endl;
    return 0;
}

void testLexer(const std::string &str) {
    using namespace std;
    using namespace CHelper;
    cout << "开始分词：" << str << endl;
    Lexer lexer = Lexer(str, "main.mcfunction");
    vector<Token> tokenList = lexer.lex();
    for (const auto &token: tokenList) {
        cout << token << endl;
    }
    cout << endl;
}
