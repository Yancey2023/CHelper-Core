//
// Created by Yancey666 on 2023/11/5.
//
#include <iostream>
#include "main.h"
#include "core/lexer/Lexer.h"


int main() {
//    testLexer("setblock minecraft:stone");
    testLexer("give asd\"ffd apple 64\" 64");
    testLexer("give @s[x=!64] minecraft:apple 64");
    testLexer("tp +0.0.1 -0.0 ~0.0 .1");
    return 0;
}

void testLexer(const std::string& str){
    std::cout << "开始分词：" << str << std::endl;
    Lexer lexer = Lexer(str, "main.mcfunction");
    std::vector<Token> tokenList = lexer.lex();
    for (const auto &token: tokenList) {
        std::cout << token << std::endl;
    }
    std::cout << std::endl;
}
