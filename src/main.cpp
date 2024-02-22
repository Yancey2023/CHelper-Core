//
// Created by Yancey666 on 2023/11/5.
//

#define CHelperLogger INFO
#define CHelperDebug true

#include "main.h"
#include <ctime>
#include "chelper/util/StringUtil.h"
#include "chelper/lexer/Lexer.h"
#include "chelper/parser/ASTNode.h"
#include "chelper/util/TokenUtil.h"

int main() {
    CHelper::Test::test(R"(D:\CLion\project\CHelper\test\test.txt)");
    return 0;
}

namespace CHelper::Test {

    void test(const std::string &testFilePath) {
        std::vector<std::string> commands;
        std::ifstream fin;
        fin.open(testFilePath, std::ios::in);
        std::string cpackPath;
        getline(fin, cpackPath);
        while (fin.is_open()) {
            std::string str;
            getline(fin, str);
            if (str.empty()) {
                break;
            }
            if (str[0] != '-') {
                commands.push_back(str);
            }
        }
        fin.close();
        CHelper::Test::test(cpackPath, commands);
    }

    void test(const std::string &cpackPath, const std::vector<std::string> &commands) {
        try {
            clock_t start, end;
            start = clock();
            auto cpack = getCPack(cpackPath);
            end = clock();
            CHELPER_INFO(ColorStringBuilder()
                                 .green("CPack load successfully (")
                                 .purple(std::to_string(end - start) + "ms")
                                 .green(")")
                                 .build());
            std::cout << std::endl;
            for (const auto &command: commands) {
                start = clock();
                const auto tokens = lex(command);
                auto parser = getParser(cpack, tokens);
                const ASTNode node = parse(parser);
                const auto description = getDescription(node, command.length() - 1);
                const auto errorReasons = getErrorReasons(cpack, node);
                const auto suggestions = getSuggestions(cpack, node, command.length() - 1);
                const auto structure = getStructure(node);
                const auto colors = getColors(node);
                end = clock();
                CHELPER_INFO(ColorStringBuilder()
                                     .green("parse successfully(")
                                     .purple(std::to_string(end - start) + "ms")
                                     .green(")")
                                     .normal(" : ")
                                     .purple(command)
                                     .build());
                CHELPER_INFO("description: " + description);
                if (errorReasons.empty()) {
                    CHELPER_INFO("no error");
                } else if (errorReasons.size() == 1) {
                    const auto &errorReason = errorReasons[0];
                    CHELPER_INFO(ColorStringBuilder()
                                         .normal("error reason: ")
                                         .red(TokenUtil::toString(errorReason->tokens) + " ")
                                         .blue(errorReason->errorReason)
                                         .build());
                } else {
                    CHELPER_INFO("error reasons:");
                    int i = 0;
                    for (const auto &item: errorReasons) {
                        CHELPER_INFO(ColorStringBuilder()
                                             .normal(std::to_string(++i) + ". ")
                                             .red(TokenUtil::toString(item->tokens) + " ")
                                             .blue(item->errorReason)
                                             .build());
                    }
                }
                CHELPER_INFO("suggestions: ");
                int i = 0;
                for (const auto &item: suggestions) {
                    if (i == 10) {
                        CHELPER_INFO("...");
                        break;
                    }
                    CHELPER_INFO(ColorStringBuilder()
                                         .normal(std::to_string(++i) + ". ")
                                         .blue(TokenUtil::toString(item.tokens) + " ")
                                         .yellow(item.content->name + " ")
                                         .normal(item.content->description.value_or(""))
                                         .build());
                }
                std::cout << std::endl;
            }
        } catch (const std::exception &e) {
            Exception::printStackTrace(e);
            exit(-1);
        }
    }

    CPack getCPack(const std::string &source) {
        return CPack::create(source);
    }

    std::vector<Token> lex(const std::string &str) {
        return Lexer(str, "unknown").lex();
    }

    Parser getParser(const CPack &cpack, const std::vector<Token> &tokenList) {
        return {TokenReader(tokenList), cpack};
    }

    ASTNode parse(Parser &parser) {
        return parser.parse();
    }

    std::string getDescription(const ASTNode &node, size_t index) {
        return node.getDescription(index);
    }

    std::vector<std::shared_ptr<ErrorReason>> getErrorReasons(const CPack &cpack, const ASTNode &node) {
        return node.getErrorReasons(cpack);
    }

    std::vector<Suggestion> getSuggestions(const CPack &cpack, const ASTNode &node, size_t index) {
        return node.getSuggestions(cpack, index);
    }

    std::string getStructure(const ASTNode &node) {
        return node.getStructure();
    }

    std::string getColors(const ASTNode &node) {
        return node.getColors();
    }

} // CHelper::Test