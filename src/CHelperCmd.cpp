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
//    CHelper::Test::test(R"(D:\CLion\project\CHelper\test\test.txt)", true);
    CHelper::Test::test(R"(D:\CLion\project\CHelper\resources)", {"give @s "}, true);
    return 0;
}

namespace CHelper::Test {

    void test(const std::string &testFilePath, bool isTestTime) {
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
        CHelper::Test::test(cpackPath, commands, isTestTime);
    }

    void test(const std::string &cpackPath, const std::vector<std::string> &commands, bool isTestTime) {
        try {
            auto core = Core::create(cpackPath);
            std::cout << std::endl;
            if (core == nullptr) {
                return;
            }
            for (const auto &command: commands) {
                clock_t startParse, endParse,
                        startDescription, endDescription,
                        startErrorReasons, endErrorReasons,
                        startSuggestions, endSuggestions,
                        startStructure, endStructure;
                startParse = clock();
                core->onTextChanged(command, command.length());
                endParse = clock();
                startDescription = clock();
                auto description = core->getDescription();
                endDescription = clock();
                startErrorReasons = clock();
                auto errorReasons = core->getErrorReasons();
                endErrorReasons = clock();
                startSuggestions = clock();
                auto suggestions = core->getSuggestions();
                endSuggestions = clock();
                startStructure = clock();
                auto structure = core->getStructure();
                endStructure = clock();
                CHELPER_INFO(ColorStringBuilder()
                                     .green("parse successfully(")
                                     .purple(std::to_string(endStructure - startParse) + "ms")
                                     .green(")")
                                     .normal(" : ")
                                     .purple(command)
                                     .build());
                if (isTestTime) {
                    CHELPER_INFO(ColorStringBuilder().blue("parse in ").purple(
                            std::to_string(endParse - startParse) + "ms").build());
                    CHELPER_INFO(ColorStringBuilder().blue("get description in ").purple(
                            std::to_string(endDescription - startDescription) + "ms").build());
                    CHELPER_INFO(ColorStringBuilder().blue("get error reasons in ").purple(
                            std::to_string(endErrorReasons - startErrorReasons) + "ms").build());
                    CHELPER_INFO(ColorStringBuilder().blue("get suggestions in ").purple(
                            std::to_string(endSuggestions - startSuggestions) + "ms").build());
                    CHELPER_INFO(ColorStringBuilder().blue("get structure in ").purple(
                            std::to_string(endStructure - startStructure) + "ms").build());

                }
                std::cout << core->getAstNode().toOptimizedJson().dump(
                        -1, ' ', true, nlohmann::detail::error_handler_t::replace) << std::endl;
                std::cout << core->getAstNode().toBestJson().dump(
                        -1, ' ', true, nlohmann::detail::error_handler_t::replace) << std::endl;
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

    void testTime(const std::string &cpackPath, const std::vector<std::string> &commands) {

    }

} // CHelper::Test