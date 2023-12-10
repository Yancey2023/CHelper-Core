//
// Created by Yancey666 on 2023/11/5.
//

#define CHelperLogger INFO

#include <ctime>
#include "main.h"
#include "core/lexer/Lexer.h"
#include "core/resources/CPack.h"
#include "core/util/Exception.h"
#include "core/util/CPackUtil.h"

using namespace std;
using namespace CHelper;
using namespace std::filesystem;

int main() {
    CHELPER_INFO("CHelper start");
    try {
        clock_t start, end;
        start = clock();
        CPack cpack = getCPack(path(R"(D:\CLion\project\CHelper\resources)"));
        end = clock();
        CHELPER_INFO(Color::BLUE + "[CHelper] " +
                     Color::GREEN + "CPack load successfully (" +
                     Color::PURPLE + to_string(end - start) + "ms" +
                     Color::GREEN + ")" +
                     Color::NORMAL);
    } catch (Exception::CPackLoadFailed &e) {
        CHELPER_ERROR(e.reason());
    }
    return 0;
}

void test() {
    //Lexer
//    testLexer("setblock minecraft:stone");
//    testLexer("give asd\"ffd apple 64\" 64");
//    testLexer("give @s[x=!64] minecraft:apple 64");
//    testLexer("tp +0.0.1 -0.0 ~0.0 .1");
}

void testLexer(const std::string &str) {
    cout << "开始分词：" << str << endl;
    Lexer lexer = Lexer(str, "unknown");
    vector<Token> tokenList = lexer.lex();
    for (const auto &token: tokenList) {
        cout << token << endl;
    }
    cout << endl;
}
