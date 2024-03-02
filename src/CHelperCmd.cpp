//
// Created by Yancey on 2023/11/5.
//

#include "CHelperCmd.h"
#include "chelper/util/StringUtil.h"
#include "chelper/lexer/Lexer.h"
#include "chelper/parser/ASTNode.h"
#include "chelper/util/TokenUtil.h"
#include "chelper/Core.h"

int main() {
    CHelper::Test::test(R"(D:\CLion\project\CHelper\test\test.txt)");
//    CHelper::Test::test(R"(D:\CLion\project\CHelper\resources)", {"execute if block ~~~ anvil[\"aaa\"=90.5] run "});
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
            auto core = Core::create(cpackPath);
            std::cout << std::endl;
            if (core == nullptr) {
                return;
            }
            for (const auto &command: commands) {
                clock_t start, end;
                start = clock();
                core->onTextChanged(command, command.length());
                auto description = core->getDescription();
                auto errorReasons = core->getErrorReasons();
                auto suggestions = core->getSuggestions();
                auto structure = core->getStructure();
                end = clock();
                CHELPER_INFO(ColorStringBuilder()
                                     .green("parse successfully(")
                                     .purple(std::to_string(end - start) + "ms")
                                     .green(")")
                                     .normal(" : ")
                                     .purple(command)
                                     .build());
//                std::cout << core->getAstNode().toOptimizedJson() << std::endl;
//                std::cout << core->getAstNode().toBestJson() << std::endl;
                CHELPER_INFO("structure: " + structure);
                CHELPER_INFO("description: " + description);
                if (errorReasons.empty()) {
                    CHELPER_INFO("no error");
                } else if (errorReasons.size() == 1) {
                    const auto &errorReason = errorReasons[0];
                    CHELPER_INFO(ColorStringBuilder()
                                         .normal("error reason: ")
                                         .red(command.substr(errorReason->start,
                                                             errorReason->end - errorReason->start) + " ")
                                         .blue(errorReason->errorReason)
                                         .build());
                    CHELPER_INFO(ColorStringBuilder()
                                         .normal(command.substr(0, errorReason->start))
                                         .red(errorReason->start == errorReason->end ? "~" :
                                              command.substr(errorReason->start,
                                                             errorReason->end - errorReason->start))
                                         .normal(command.substr(errorReason->end))
                                         .build());
                } else {
                    CHELPER_INFO("error reasons:");
                    int i = 0;
                    for (const auto &errorReason: errorReasons) {
                        CHELPER_INFO(ColorStringBuilder()
                                             .normal(std::to_string(++i) + ". ")
                                             .red(command.substr(errorReason->start,
                                                                 errorReason->end - errorReason->start) + " ")
                                             .blue(errorReason->errorReason)
                                             .build());
                        CHELPER_INFO(ColorStringBuilder()
                                             .normal(command.substr(0, errorReason->start))
                                             .red(errorReason->start == errorReason->end ? "~" :
                                                  command.substr(errorReason->start,
                                                                 errorReason->end - errorReason->start))
                                             .normal(command.substr(errorReason->end))
                                             .build());
                    }
                }
                if (suggestions.empty()) {
                    CHELPER_INFO("no suggestion");
                } else {
                    CHELPER_INFO("suggestions: ");
                    int i = 0;
                    for (const auto &item: suggestions) {
                        if (i == 30) {
                            CHELPER_INFO("...");
                            break;
                        }
                        CHELPER_INFO(ColorStringBuilder()
                                             .normal(std::to_string(++i) + ". ")
                                             .green(item.content->name + " ")
                                             .blue(item.content->description.value_or(""))
                                             .build());
                        CHELPER_INFO(ColorStringBuilder()
                                             .normal(command.substr(0, item.start))
                                             .green(item.content->name)
                                             .normal(command.substr(item.end))
                                             .build());
                    }
                }
                std::cout << std::endl;
            }
        } catch (const std::exception &e) {
            Exception::printStackTrace(e);
            Profile::clear();
            exit(-1);
        }
    }

} // CHelper::Test