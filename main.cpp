//
// Created by Yancey666 on 2023/11/5.
//

#define CHelperLogger INFO
#define CHelperDebug true

#include <ctime>
#include "main.h"
#include "core/util/Exception.h"
#include "core/resources/command/node/NodeCommand.h"
#include "core/parser/Parser.h"
#include "core/util/StringUtil.h"

int main() {
    test();
    return 0;
}

void test() {
    CHelper::CPack cpack = getCPack(R"(D:\CLion\project\CHelper\resources)");
    std::vector<std::string> commands = {
            "ppp a",
//            "setblock aaa minecraft:stone",
//            "setblock ~1^2~3 minecraft:stone",
            "give @s stone 1"
    };
    for (const auto &item: commands) {
        testParser(cpack, item);
    }
}

void testLexer(const std::string &str) {
    CHELPER_INFO(CHelper::Color::NORMAL + "start lex: " +
                 CHelper::Color::PURPLE + str +
                 CHelper::Color::NORMAL);
    CHelper::Lexer lexer = CHelper::Lexer(str, "unknown");
    std::vector<CHelper::Token> tokenList = lexer.lex();
    for (const auto &token: tokenList) {
        std::cout << token << std::endl;
    }
    std::cout << std::endl;
}

void testJsonCPack(const std::string &source) {
    CHELPER_INFO(CHelper::Color::NORMAL + "start load CPack by JSON: " +
                 CHelper::Color::PURPLE + source +
                 CHelper::Color::NORMAL);
    try {
        clock_t start, end;
        start = clock();
        CHelper::CPack cpack((std::filesystem::path(source)));
        end = clock();
        CHELPER_INFO(CHelper::Color::BLUE + "[CHelper] " +
                     CHelper::Color::GREEN + "CPack load successfully (" +
                     CHelper::Color::PURPLE + std::to_string(end - start) + "ms" +
                     CHelper::Color::GREEN + ")" +
                     CHelper::Color::NORMAL);
    } catch (CHelper::Exception::CPackLoadFailed &e) {
        CHELPER_ERROR(e.reason());
    }
    std::cout << std::endl;
}

CHelper::CPack getCPack(const std::string &source) {
    try {
        //加载CPack
        CHELPER_INFO(CHelper::Color::NORMAL + "start load CPack by JSON: " +
                     CHelper::Color::PURPLE + source +
                     CHelper::Color::NORMAL);
        clock_t start, end;
        start = clock();
        CHelper::CPack cpack((std::filesystem::path(source)));
        end = clock();
        CHELPER_INFO(CHelper::Color::BLUE + "[CHelper] " +
                     CHelper::Color::GREEN + "CPack load successfully (" +
                     CHelper::Color::PURPLE + std::to_string(end - start) + "ms" +
                     CHelper::Color::GREEN + ")" +
                     CHelper::Color::NORMAL);
        std::cout << std::endl;
        return cpack;
    } catch (CHelper::Exception::CPackLoadFailed &e) {
        CHELPER_ERROR(e.reason());
        exit(-1);
    }
}

void testParser(const CHelper::CPack &cpack, const std::string &command) {
    //lexer
    clock_t start, end;
    start = clock();
    CHelper::Lexer lexer = CHelper::Lexer(command, "unknown");
    std::vector<CHelper::Token> tokenList = lexer.lex();
    end = clock();

    //parser
    CHelper::Parser parser(CHelper::TokenReader(tokenList), cpack);
    CHelper::ASTNode node = parser.parse();

    CHELPER_INFO(CHelper::Color::GREEN + "parse successfully(" +
                 CHelper::Color::PURPLE + std::to_string(end - start) + "ms" +
                 CHelper::Color::GREEN + ")" +
                 CHelper::Color::NORMAL + " : " +
                 CHelper::Color::PURPLE + command +
                 CHelper::Color::NORMAL);
//    for (const auto &token: tokenList) {
//        std::cout << token << std::endl;
//    }
//    std::cout << std::endl;
    std::cout << node << std::endl;
    if (!node.isError()) {
        CHELPER_INFO("no error");
    } else if (node.errorReasons.size() == 1) {
        std::shared_ptr<CHelper::ErrorReason> errorReason = node.errorReasons[0];
        CHELPER_INFO(CHelper::Color::NORMAL + "error reason: " +
                     CHelper::Color::RED + CHelper::StringUtil::toString(errorReason->tokens) + " " +
                     CHelper::Color::BLUE + errorReason->errorReason +
                     CHelper::Color::NORMAL);
    } else {
        CHELPER_INFO("error reason:");
        int i = 0;
        for (const auto &item: node.errorReasons) {
            CHELPER_INFO(CHelper::Color::NORMAL + std::to_string(++i) + ". " +
                         CHelper::Color::RED + CHelper::StringUtil::toString(item->tokens) + " " +
                         CHelper::Color::BLUE + item->errorReason +
                         CHelper::Color::NORMAL);
        }
    }
    std::cout << std::endl;
}