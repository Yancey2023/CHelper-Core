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
//    CHelper::Test::test(R"(D:\CLion\project\CHelper\resources)",
//                        R"(D:\CLion\project\CHelper\test\test.txt)",
//                        false);
    CHelper::Test::test(R"(/home/yancey/CLionProjects/CHelper/resources)",
                        R"(/home/yancey/CLionProjects/CHelper/test/test.txt)",
                        false);
//    CHelper::Test::test(R"(D:\CLion\project\CHelper\resources)", {"give @s "}, false);
//    CHelper::Test::test(R"(/home/yancey/CLionProjects/CHelper/resources)", {"give @s "}, false);
//    CHelper::Test::test(R"(D:\CLion\project\CHelper\resources)", {""}, false);
    return 0;
}

namespace CHelper::Test {

    void test(const std::string &cpackPath, const std::string &testFilePath, bool isTestTime) {
        std::vector<std::string> commands;
        std::ifstream fin;
        fin.open(testFilePath, std::ios::in);
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
//        std::vector<std::string> commands1;
//        for (const auto &item: commands) {
//            for (size_t i = 0; i < item.size(); i++) {
//                commands1.push_back(item.substr(0, i + 1));
//            }
//        }
//        CHelper::Test::test(cpackPath, commands1, isTestTime);
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
                std::chrono::high_resolution_clock::time_point startParse, endParse,
                        startDescription, endDescription,
                        startErrorReasons, endErrorReasons,
                        startSuggestions, endSuggestions,
                        startStructure, endStructure;
                startParse = std::chrono::high_resolution_clock::now();
                core->onTextChanged(command, command.length());
                endParse = std::chrono::high_resolution_clock::now();
                startDescription = std::chrono::high_resolution_clock::now();
                auto description = core->getDescription();
                endDescription = std::chrono::high_resolution_clock::now();
                startErrorReasons = std::chrono::high_resolution_clock::now();
                auto errorReasons = core->getErrorReasons();
                endErrorReasons = std::chrono::high_resolution_clock::now();
                startSuggestions = std::chrono::high_resolution_clock::now();
                auto suggestions = core->getSuggestions();
                endSuggestions = std::chrono::high_resolution_clock::now();
                startStructure = std::chrono::high_resolution_clock::now();
                auto structure = core->getStructure();
                endStructure = std::chrono::high_resolution_clock::now();
                CHELPER_INFO(ColorStringBuilder()
                                     .green("parse successfully(")
                                     .purple(std::to_string(std::chrono::duration_cast<std::chrono::milliseconds>(
                                             endStructure - startParse).count()) + "ms")
                                     .green(")")
                                     .normal(" : ")
                                     .purple(command)
                                     .build());
                if (isTestTime) {
                    CHELPER_INFO(ColorStringBuilder().blue("parse in ").purple(
                            std::to_string(std::chrono::duration_cast<std::chrono::milliseconds>(
                                    endParse - startParse).count()) + "ms" + "ms").build());
                    CHELPER_INFO(ColorStringBuilder().blue("get description in ").purple(
                            std::to_string(std::chrono::duration_cast<std::chrono::milliseconds>(
                                    endDescription - startDescription).count()) + "ms" + "ms").build());
                    CHELPER_INFO(ColorStringBuilder().blue("get error reasons in ").purple(
                            std::to_string(std::chrono::duration_cast<std::chrono::milliseconds>(
                                    endErrorReasons - startErrorReasons).count()) + "ms" + "ms").build());
                    CHELPER_INFO(ColorStringBuilder().blue("get suggestions in ").purple(
                            std::to_string(std::chrono::duration_cast<std::chrono::milliseconds>(
                                    endSuggestions - startSuggestions).count()) + "ms" + "ms").build());
                    CHELPER_INFO(ColorStringBuilder().blue("get structure in ").purple(
                            std::to_string(std::chrono::duration_cast<std::chrono::milliseconds>(
                                    endStructure - startStructure).count()) + "ms" + "ms").build());

                }
//                std::cout << core->getAstNode().toOptimizedJson().dump(
//                        -1, ' ', false, nlohmann::detail::error_handler_t::replace) << std::endl;
//                std::cout << core->getAstNode().toBestJson().dump(
//                        -1, ' ', false, nlohmann::detail::error_handler_t::replace) << std::endl;
                CHELPER_INFO("structure: " + structure);
                CHELPER_INFO("description: " + description);
                if (errorReasons.empty()) {
                    CHELPER_INFO("no error");
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