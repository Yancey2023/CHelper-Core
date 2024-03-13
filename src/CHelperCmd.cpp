//
// Created by Yancey on 2023/11/5.
//

#include "CHelperCmd.h"
#include "chelper/util/StringUtil.h"
#include "chelper/lexer/Lexer.h"
#include "chelper/Core.h"

int main() {
    CHelper::Test::test(R"(D:\CLion\project\CHelper-Core\resources)",
                        R"(D:\CLion\project\CHelper-Core\test\test.txt)",
                        false);
//    CHelper::Test::test(R"(/home/yancey/CLionProjects/CHelper/resources)",
//                        R"(/home/yancey/CLionProjects/CHelper/test/test.txt)",
//                        true);
//    CHelper::Test::test(R"(D:\CLion\project\CHelper-Core\resources)",
//                        std::vector<std::string>{"give @s "}, false);
//    CHelper::Test::test(R"(/home/yancey/CLionProjects/CHelper/resources)",
//                        std::vector<std::string>{"give @s "}, true);
//    CHelper::Test::test(R"(D:\CLion\project\CHelper-Core\resources)", {""}, false);
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
            if (str[0] == '-') {
                continue;
            }
            if (str[str.length() - 1] == '\r') {
                str = str.substr(0, str.length() - 1);
            }
            commands.push_back(str);
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
//        CHelper::Test::test2(cpackPath, commands, 500);
//        CHelper::Test::test2(cpackPath, commands, 1);
//        CHelper::Test::test2(cpackPath, commands, 100);
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
                std::cout << ColorStringBuilder()
                        .green("parse successfully(")
                        .purple(std::to_string(std::chrono::duration_cast<std::chrono::duration<float, std::milli>>(
                                endStructure - startParse).count()) + "ms")
                        .green(")")
                        .normal(" : ")
                        .purple(command)
                        .build() << std::endl;
                if (isTestTime) {
                    std::cout << ColorStringBuilder().blue("parse in ").purple(
                            std::to_string(std::chrono::duration_cast<std::chrono::duration<float, std::milli>>(
                                    endParse - startParse).count()) + "ms").build() << std::endl;
                    std::cout << ColorStringBuilder().blue("get description in ").purple(
                            std::to_string(std::chrono::duration_cast<std::chrono::duration<float, std::milli>>(
                                    endDescription - startDescription).count()) + "ms").build() << std::endl;
                    std::cout << ColorStringBuilder().blue("get error reasons in ").purple(
                            std::to_string(std::chrono::duration_cast<std::chrono::duration<float, std::milli>>(
                                    endErrorReasons - startErrorReasons).count()) + "ms").build() << std::endl;
                    std::cout << ColorStringBuilder().blue("get suggestions in ").purple(
                            std::to_string(std::chrono::duration_cast<std::chrono::duration<float, std::milli>>(
                                    endSuggestions - startSuggestions).count()) + "ms").build() << std::endl;
                    std::cout << ColorStringBuilder().blue("get structure in ").purple(
                            std::to_string(std::chrono::duration_cast<std::chrono::duration<float, std::milli>>(
                                    endStructure - startStructure).count()) + "ms").build() << std::endl;

                }
//                std::cout << core->getAstNode().toOptimizedJson().dump(
//                        -1, ' ', false, nlohmann::detail::error_handler_t::replace) << std::endl;
//                std::cout << core->getAstNode().toBestJson().dump(
//                        -1, ' ', false, nlohmann::detail::error_handler_t::replace) << std::endl;
                std::cout << "structure: " + structure << std::endl;
                std::cout << "description: " + description << std::endl;
                if (errorReasons.empty()) {
                    std::cout << "no error" << std::endl;
                } else {
                    std::cout << "error reasons:" << std::endl;
                    int i = 0;
                    for (const auto &errorReason: errorReasons) {
                        std::cout << ColorStringBuilder()
                                .normal(std::to_string(++i) + ". ")
                                .red(command.substr(errorReason->start,
                                                    errorReason->end - errorReason->start) + " ")
                                .blue(errorReason->errorReason)
                                .build() << std::endl;
                        std::cout << ColorStringBuilder()
                                .normal(command.substr(0, errorReason->start))
                                .red(errorReason->start == errorReason->end ? "~" :
                                     command.substr(errorReason->start,
                                                    errorReason->end - errorReason->start))
                                .normal(command.substr(errorReason->end))
                                .build() << std::endl;
                    }
                }
                if (suggestions->empty()) {
                    std::cout << "no suggestion" << std::endl;
                } else {
                    std::cout << "suggestions: " << std::endl;
                    int i = 0;
                    for (const auto &item: *suggestions) {
                        if (i == 30) {
                            std::cout << "..." << std::endl;
                            break;
                        }
                        std::cout << ColorStringBuilder()
                                .normal(std::to_string(++i) + ". ")
                                .green(item.content->name + " ")
                                .blue(item.content->description.value_or(""))
                                .build() << std::endl;
                        std::cout << ColorStringBuilder()
                                .normal(command.substr(0, item.start))
                                .green(item.content->name)
                                .normal(command.substr(item.end))
                                .build() << std::endl;
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

    void test2(const std::string &cpackPath, const std::vector<std::string> &commands, int times) {
        try {
            auto core = Core::create(cpackPath);
            std::cout << std::endl;
            if (core == nullptr) {
                return;
            }
            std::chrono::high_resolution_clock::time_point start, end;
            start = std::chrono::high_resolution_clock::now();
            for (int i = 0; i < times; ++i) {
                for (const auto &command: commands) {
                    core->onTextChanged(command, command.length());
                    auto description = core->getDescription();
                    auto errorReasons = core->getErrorReasons();
                    core->getSuggestions();
                    auto structure = core->getStructure();
                }
            }
            end = std::chrono::high_resolution_clock::now();
            std::cout << ColorStringBuilder()
                    .green("parse successfully(")
                    .purple(std::to_string(std::chrono::duration_cast<std::chrono::duration<float, std::milli>>(
                            end - start).count()) + "ms")
                    .green(")")
                    .build() << std::endl;
        } catch (const std::exception &e) {
            Exception::printStackTrace(e);
            Profile::clear();
            exit(-1);
        }
    }

} // CHelper::Test