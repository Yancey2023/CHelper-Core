//
// Created by Yancey on 2023/11/5.
//

#include "CHelperCmd.h"
#include "../chelper/Core.h"

int main() {
    //    testDir();
    //    testBin();
    outputSingleJson();
    outputBson();
    outputBinary();
    return 0;
}

[[maybe_unused]] void testDir() {
#ifdef _WIN32
    CHelper::Test::testDir(R"(D:\CLion\project\CHelper-Core\resources\beta\vanilla)",
                           R"(D:\CLion\project\CHelper-Core\test\test.txt)",
                           false);
    //    CHelper::Test::testDir(R"(D:\CLion\project\CHelper-Core\resources\beta\vanilla)",
    //                        std::vector<std::string>{"execute run clear "}, false);
    //    CHelper::Test::testDir(R"(D:\CLion\project\CHelper-Core\resources\beta\vanilla)", {""}, false);
    //    CHelper::Test::test4(R"(D:\CLion\project\CHelper-Core\resources)",
    //                         R"(D:\CLion\project\CHelper-Core\run\cpack.json)");
#else
    CHelper::Test::testDir(R"(/home/yancey/CLionProjects/CHelper-Core/resources/beta/vanilla)",
                           R"(/home/yancey/CLionProjects/CHelper-Core/test/test.txt)",
                           true);
    //    CHelper::Test::testDir(R"(/home/yancey/CLionProjects/CHelper-Core/resources/beta/vanilla)",
    //                        std::vector<std::string>{"give @s "}, true);
#endif
}

[[maybe_unused]] void testBin() {
#ifdef _WIN32
    CHelper::Test::testBin(R"(D:\CLion\project\CHelper-Core\run\beta-experiment-1.21.0.23.cpack)",
                           R"(D:\CLion\project\CHelper-Core\test\test.txt)",
                           false);
#else
    CHelper::Test::testBin(R"(/home/yancey/CLionProjects/CHelper-Core/run/beta-experiment-1.21.0.23.cpack)",
                           R"(/home/yancey/CLionProjects/CHelper-Core/test/test.txt)",
                           true);
    //    for (int i = 0; i < 100; ++i) {
    //        CHelper::Core::createByBinary("/home/yancey/CLionProjects/CHelper-Core/run/beta-experiment-1.21.0.23.cpack");
    //    }
#endif
}

[[maybe_unused]] void outputSingleJson() {
#ifdef _WIN32
    // release
    CHelper::Test::writeSingleJson(R"(D:\CLion\project\CHelper-Core\resources\release\vanilla)",
                                   R"(D:\CLion\project\CHelper-Core\run\release-vanilla-1.20.80.05.json)");
    CHelper::Test::writeSingleJson(R"(D:\CLion\project\CHelper-Core\resources\release\experiment)",
                                   R"(D:\CLion\project\CHelper-Core\run\release-experiment-1.20.80.05.json)");
    // beta
    CHelper::Test::writeSingleJson(R"(D:\CLion\project\CHelper-Core\resources\beta\vanilla)",
                                   R"(D:\CLion\project\CHelper-Core\run\beta-vanilla-1.21.0.23.json)");
    CHelper::Test::writeSingleJson(R"(D:\CLion\project\CHelper-Core\resources\beta\experiment)",
                                   R"(D:\CLion\project\CHelper-Core\run\beta-experiment-1.21.0.23.json)");
    // netease
    //    CHelper::Test::writeSingleJson(R"(D:\CLion\project\CHelper-Core\resources\netease\vanilla)",
    //                             R"(D:\CLion\project\CHelper-Core\run\netease-1.21.0.23.cpack)");
    //    CHelper::Test::writeSingleJson(R"(D:\CLion\project\CHelper-Core\resources\netease\experiment)",
    //                             R"(D:\CLion\project\CHelper-Core\run\netease-1.21.0.23.cpack)");
#else
    // release
    CHelper::Test::writeSingleJson(R"(/home/yancey/CLionProjects/CHelper-Core/resources/release/vanilla)",
                                   R"(/home/yancey/CLionProjects/CHelper-Core/run/release-vanilla-1.20.80.05.json)");
    CHelper::Test::writeSingleJson(R"(/home/yancey/CLionProjects/CHelper-Core/resources/release/experiment)",
                                   R"(/home/yancey/CLionProjects/CHelper-Core/run/release-experiment-1.20.80.05.json)");
    // beta
    CHelper::Test::writeSingleJson(R"(/home/yancey/CLionProjects/CHelper-Core/resources/beta/vanilla)",
                                   R"(/home/yancey/CLionProjects/CHelper-Core/run/beta-vanilla-1.21.0.23.json)");
    CHelper::Test::writeSingleJson(R"(/home/yancey/CLionProjects/CHelper-Core/resources/beta/experiment)",
                                   R"(/home/yancey/CLionProjects/CHelper-Core/run/beta-experiment-1.21.0.23.json)");
    // netease
    //    CHelper::Test::writeSingleJson(R"(/home/yancey/CLionProjects/CHelper-Core/resources/netease/vanilla)",
    //                               R"(/home/yancey/CLionProjects/CHelper-Core/run/netease-1.21.0.23.json)");
    //    CHelper::Test::writeSingleJson(R"(/home/yancey/CLionProjects/CHelper-Core/resources/netease/experiment)",
    //                               R"(/home/yancey/CLionProjects/CHelper-Core/run/netease-1.21.0.23.json)");
#endif
}

[[maybe_unused]] void outputBson() {
#ifdef _WIN32
    // release
    CHelper::Test::writeBson(R"(D:\CLion\project\CHelper-Core\resources\release\vanilla)",
                             R"(D:\CLion\project\CHelper-Core\run\release-vanilla-1.20.80.05.bson)");
    CHelper::Test::writeBson(R"(D:\CLion\project\CHelper-Core\resources\release\experiment)",
                             R"(D:\CLion\project\CHelper-Core\run\release-experiment-1.20.80.05.bson)");
    // beta
    CHelper::Test::writeBson(R"(D:\CLion\project\CHelper-Core\resources\beta\vanilla)",
                             R"(D:\CLion\project\CHelper-Core\run\beta-vanilla-1.21.0.23.bson)");
    CHelper::Test::writeBson(R"(D:\CLion\project\CHelper-Core\resources\beta\experiment)",
                             R"(D:\CLion\project\CHelper-Core\run\beta-experiment-1.21.0.23.bson)");
    // netease
    //    CHelper::Test::writeBson(R"(D:\CLion\project\CHelper-Core\resources\netease\vanilla)",
    //                             R"(D:\CLion\project\CHelper-Core\run\netease-1.21.0.23.bson)");
    //    CHelper::Test::writeBson(R"(D:\CLion\project\CHelper-Core\resources\netease\experiment)",
    //                             R"(D:\CLion\project\CHelper-Core\run\netease-1.21.0.23.bson)");
#else
    // release
    CHelper::Test::writeBson(R"(/home/yancey/CLionProjects/CHelper-Core/resources/release/vanilla)",
                             R"(/home/yancey/CLionProjects/CHelper-Core/run/release-vanilla-1.20.80.05.bson)");
    CHelper::Test::writeBson(R"(/home/yancey/CLionProjects/CHelper-Core/resources/release/experiment)",
                             R"(/home/yancey/CLionProjects/CHelper-Core/run/release-experiment-1.20.80.05.bson)");
    // beta
    CHelper::Test::writeBson(R"(/home/yancey/CLionProjects/CHelper-Core/resources/beta/vanilla)",
                             R"(/home/yancey/CLionProjects/CHelper-Core/run/beta-vanilla-1.21.0.23.bson)");
    CHelper::Test::writeBson(R"(/home/yancey/CLionProjects/CHelper-Core/resources/beta/experiment)",
                             R"(/home/yancey/CLionProjects/CHelper-Core/run/beta-experiment-1.21.0.23.bson)");
    // netease
    //    CHelper::Test::writeBson(R"(/home/yancey/CLionProjects/CHelper-Core/resources/netease/vanilla)",
    //                               R"(/home/yancey/CLionProjects/CHelper-Core/run/netease-1.21.0.23.bson)");
    //    CHelper::Test::writeBson(R"(/home/yancey/CLionProjects/CHelper-Core/resources/netease/experiment)",
    //                               R"(/home/yancey/CLionProjects/CHelper-Core/run/netease-1.21.0.23.bson)");
#endif
}

[[maybe_unused]] void outputBinary() {
#ifdef _WIN32
    // release
    CHelper::Test::writeBinary(R"(D:\CLion\project\CHelper-Core\resources\release\vanilla)",
                               R"(D:\CLion\project\CHelper-Core\run\release-vanilla-1.20.80.05.cpack)");
    CHelper::Test::writeBinary(R"(D:\CLion\project\CHelper-Core\resources\release\experiment)",
                               R"(D:\CLion\project\CHelper-Core\run\release-experiment-1.20.80.05.cpack)");
    // beta
    CHelper::Test::writeBinary(R"(D:\CLion\project\CHelper-Core\resources\beta\vanilla)",
                               R"(D:\CLion\project\CHelper-Core\run\beta-vanilla-1.21.0.23.cpack)");
    CHelper::Test::writeBinary(R"(D:\CLion\project\CHelper-Core\resources\beta\experiment)",
                               R"(D:\CLion\project\CHelper-Core\run\beta-experiment-1.21.0.23.cpack)");
    // netease
    // CHelper::Test::writeBinary(R"(D:\CLion\project\CHelper-Core\resources\netease\vanilla)",
    //                            R"(D:\CLion\project\CHelper-Core\run\netease-1.21.0.23.cpack)");
    // CHelper::Test::writeBinary(R"(D:\CLion\project\CHelper-Core\resources\netease\experiment)",
    //                            R"(D:\CLion\project\CHelper-Core\run\netease-1.21.0.23.cpack)");
#else
    // release
    CHelper::Test::writeBinary(R"(/home/yancey/CLionProjects/CHelper-Core/resources/release/vanilla)",
                               R"(/home/yancey/CLionProjects/CHelper-Core/run/release-vanilla-1.20.80.05.cpack)");
    CHelper::Test::writeBinary(R"(/home/yancey/CLionProjects/CHelper-Core/resources/release/experiment)",
                               R"(/home/yancey/CLionProjects/CHelper-Core/run/release-experiment-1.20.80.05.cpack)");
    // beta
    CHelper::Test::writeBinary(R"(/home/yancey/CLionProjects/CHelper-Core/resources/beta/vanilla)",
                               R"(/home/yancey/CLionProjects/CHelper-Core/run/beta-vanilla-1.21.0.23.cpack)");
    CHelper::Test::writeBinary(R"(/home/yancey/CLionProjects/CHelper-Core/resources/beta/experiment)",
                               R"(/home/yancey/CLionProjects/CHelper-Core/run/beta-experiment-1.21.0.23.cpack)");
    // netease
    //    CHelper::Test::writeBinary(R"(/home/yancey/CLionProjects/CHelper-Core/resources/netease/vanilla)",
    //                               R"(/home/yancey/CLionProjects/CHelper-Core/run/netease-1.21.0.23.cpack)");
    //    CHelper::Test::writeBinary(R"(/home/yancey/CLionProjects/CHelper-Core/resources/netease/experiment)",
    //                               R"(/home/yancey/CLionProjects/CHelper-Core/run/netease-1.21.0.23.cpack)");
#endif
}

namespace CHelper::Test {

    /**
     * 读取测试文件进行测试
     */
    [[maybe_unused]] void testDir(const std::string &cpackPath, const std::string &testFilePath, bool isTestTime) {
        std::vector<std::string> commands;
        std::ifstream fin;
        fin.open(testFilePath, std::ios::in);
        while (fin.is_open()) {
            std::string str;
            getline(fin, str);
            if (HEDLEY_UNLIKELY(str.empty())) {
                break;
            }
            if (HEDLEY_LIKELY(str[0] == '-')) {
                continue;
            }
            if (HEDLEY_UNLIKELY(str[str.length() - 1] == '\r')) {
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
        //        CHelper::Test::testDir(cpackPath, commands1, isTestTime);
        CHelper::Test::testDir(cpackPath, commands, isTestTime);
        //        CHelper::Test::test2(cpackPath, commands, 500);
        //        CHelper::Test::test2(cpackPath, commands, 1);
        //        CHelper::Test::test2(cpackPath, commands, 100);
    }

    /**
     * 读取测试文件进行测试
     */
    [[maybe_unused]] void testBin(const std::string &cpackPath, const std::string &testFilePath, bool isTestTime) {
        std::vector<std::string> commands;
        std::ifstream fin;
        fin.open(testFilePath, std::ios::in);
        while (fin.is_open()) {
            std::string str;
            getline(fin, str);
            if (HEDLEY_UNLIKELY(str.empty())) {
                break;
            }
            if (HEDLEY_LIKELY(str[0] == '-')) {
                continue;
            }
            if (HEDLEY_UNLIKELY(str[str.length() - 1] == '\r')) {
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
        //        CHelper::Test::testDir(cpackPath, commands1, isTestTime);
        CHelper::Test::testBin(cpackPath, commands, isTestTime);
        //        CHelper::Test::test2(cpackPath, commands, 500);
        //        CHelper::Test::test2(cpackPath, commands, 1);
        //        CHelper::Test::test2(cpackPath, commands, 100);
    }

    /**
     * 测试程序是否可以正常运行
     */
    [[maybe_unused]] void
    testDir(const std::string &cpackPath, const std::vector<std::string> &commands, bool isTestTime) {
        Core *core;
        try {
            core = Core::createByDirectory(cpackPath);
            std::cout << std::endl;
            if (HEDLEY_UNLIKELY(core == nullptr)) {
                return;
            }
        } catch (const std::exception &e) {
            Exception::printStackTrace(e);
            Profile::clear();
            exit(-1);
        }
        test(core, commands, isTestTime);
    }

    /**
     * 测试程序是否可以正常运行
     */
    [[maybe_unused]] void
    testBin(const std::string &cpackPath, const std::vector<std::string> &commands, bool isTestTime) {
        Core *core;
        try {
            core = Core::createByBinary(cpackPath);
            std::cout << std::endl;
            if (HEDLEY_UNLIKELY(core == nullptr)) {
                return;
            }
        } catch (const std::exception &e) {
            Exception::printStackTrace(e);
            Profile::clear();
            exit(-1);
        }
        test(core, commands, isTestTime);
    }

    /**
     * 测试程序是否可以正常运行
     */
    [[maybe_unused]] void
    test(Core *core, const std::vector<std::string> &commands, bool isTestTime) {
        try {
            if (HEDLEY_UNLIKELY(core == nullptr)) {
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
                                                                    endStructure - startParse)
                                                                    .count()) +
                                             "ms")
                                     .green(")")
                                     .normal(" : ")
                                     .purple(command)
                                     .build()
                          << std::endl;
                if (isTestTime) {
                    std::cout << ColorStringBuilder().blue("parse in ").purple(std::to_string(std::chrono::duration_cast<std::chrono::duration<float, std::milli>>(endParse - startParse).count()) + "ms").build() << std::endl;
                    std::cout << ColorStringBuilder().blue("get description in ").purple(std::to_string(std::chrono::duration_cast<std::chrono::duration<float, std::milli>>(endDescription - startDescription).count()) + "ms").build() << std::endl;
                    std::cout << ColorStringBuilder().blue("get error reasons in ").purple(std::to_string(std::chrono::duration_cast<std::chrono::duration<float, std::milli>>(endErrorReasons - startErrorReasons).count()) + "ms").build() << std::endl;
                    std::cout << ColorStringBuilder().blue("get suggestions in ").purple(std::to_string(std::chrono::duration_cast<std::chrono::duration<float, std::milli>>(endSuggestions - startSuggestions).count()) + "ms").build() << std::endl;
                    std::cout << ColorStringBuilder().blue("get structure in ").purple(std::to_string(std::chrono::duration_cast<std::chrono::duration<float, std::milli>>(endStructure - startStructure).count()) + "ms").build() << std::endl;
                }
                std::cout << core->getAstNode()->toJson().dump(-1, ' ', false, nlohmann::detail::error_handler_t::replace)
                          << std::endl;
                std::cout << core->getAstNode()->toBestJson().dump(-1, ' ', false, nlohmann::detail::error_handler_t::replace)
                          << std::endl;
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
                                                                 errorReason->end - errorReason->start) +
                                                  " ")
                                             .blue(errorReason->errorReason)
                                             .build()
                                  << std::endl;
                        std::cout << ColorStringBuilder()
                                             .normal(command.substr(0, errorReason->start))
                                             .red(errorReason->start == errorReason->end ? "~" : command.substr(errorReason->start, errorReason->end - errorReason->start))
                                             .normal(command.substr(errorReason->end))
                                             .build()
                                  << std::endl;
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
                                             .build()
                                  << std::endl;
                        std::string result = command.substr(0, item.start)
                                                     .append(item.content->name)
                                                     .append(command.substr(item.end));
                        std::string greenPart = item.content->name;
                        if (item.end == command.length()) {
                            ASTNode astNode = Parser::parse(result, core->getCPack());
                            if (item.isAddWhitespace && astNode.isAllWhitespaceError()) {
                                greenPart.push_back(' ');
                            }
                        }
                        std::cout << ColorStringBuilder()
                                             .normal(command.substr(0, item.start))
                                             .green(greenPart)
                                             .normal(command.substr(item.end))
                                             .build()
                                  << std::endl;
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

    /**
     * 测试程序性能
     */
    [[maybe_unused]] void test2(const std::string &cpackPath, const std::vector<std::string> &commands, int times) {
        try {
            auto core = Core::createByDirectory(cpackPath);
            std::cout << std::endl;
            if (HEDLEY_UNLIKELY(core == nullptr)) {
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
                                                                end - start)
                                                                .count()) +
                                         "ms")
                                 .green(")")
                                 .build()
                      << std::endl;
        } catch (const std::exception &e) {
            Exception::printStackTrace(e);
            Profile::clear();
            exit(-1);
        }
    }

    [[maybe_unused]] void writeDirectory(const std::string &input, const std::string &output) {
        try {
            auto core = Core::createByDirectory(input);
            std::cout << std::endl;
            if (HEDLEY_UNLIKELY(core == nullptr)) {
                return;
            }
            std::chrono::high_resolution_clock::time_point start, end;
            start = std::chrono::high_resolution_clock::now();
            core->getCPack()->writeJsonToDirectory(output);
            end = std::chrono::high_resolution_clock::now();
            std::cout << ColorStringBuilder()
                                 .green("write successfully(")
                                 .purple(std::to_string(std::chrono::duration_cast<std::chrono::duration<float, std::milli>>(
                                                                end - start)
                                                                .count()) +
                                         "ms")
                                 .green(")")
                                 .build()
                      << std::endl;
            auto core2 = Core::createByDirectory(output);
        } catch (const std::exception &e) {
            Exception::printStackTrace(e);
            Profile::clear();
            exit(-1);
        }
    }

    [[maybe_unused]] void writeSingleJson(const std::string &input, const std::string &output) {
        try {
            auto core = Core::createByDirectory(input);
            std::cout << std::endl;
            if (HEDLEY_UNLIKELY(core == nullptr)) {
                return;
            }
            std::chrono::high_resolution_clock::time_point start, end;
            start = std::chrono::high_resolution_clock::now();
            core->getCPack()->writeJsonToFile(output);
            end = std::chrono::high_resolution_clock::now();
            std::cout << ColorStringBuilder()
                                 .green("write successfully(")
                                 .purple(std::to_string(std::chrono::duration_cast<std::chrono::duration<float, std::milli>>(
                                                                end - start)
                                                                .count()) +
                                         "ms")
                                 .green(")")
                                 .build()
                      << std::endl;
            std::cout << std::endl;
            auto core2 = Core::createByJson(output);
            std::cout << std::endl;
        } catch (const std::exception &e) {
            Exception::printStackTrace(e);
            Profile::clear();
            exit(-1);
        }
    }

    [[maybe_unused]] void writeBson(const std::string &input, const std::string &output) {
        try {
            auto core = Core::createByDirectory(input);
            std::cout << std::endl;
            if (HEDLEY_UNLIKELY(core == nullptr)) {
                return;
            }
            std::chrono::high_resolution_clock::time_point start, end;
            start = std::chrono::high_resolution_clock::now();
            core->getCPack()->writeBsonToFile(output);
            end = std::chrono::high_resolution_clock::now();
            std::cout << ColorStringBuilder()
                                 .green("write successfully(")
                                 .purple(std::to_string(std::chrono::duration_cast<std::chrono::duration<float, std::milli>>(
                                                                end - start)
                                                                .count()) +
                                         "ms")
                                 .green(")")
                                 .build()
                      << std::endl;
            std::cout << std::endl;
            auto core2 = Core::createByBson(output);
            std::cout << std::endl;
        } catch (const std::exception &e) {
            Exception::printStackTrace(e);
            Profile::clear();
            exit(-1);
        }
    }

    [[maybe_unused]] void writeBinary(const std::string &input, const std::string &output) {
        try {
            auto core = Core::createByDirectory(input);
            std::cout << std::endl;
            if (HEDLEY_UNLIKELY(core == nullptr)) {
                return;
            }
            std::chrono::high_resolution_clock::time_point start, end;
            start = std::chrono::high_resolution_clock::now();
            core->getCPack()->writeBinToFile(output);
            end = std::chrono::high_resolution_clock::now();
            std::cout << ColorStringBuilder()
                                 .green("write successfully(")
                                 .purple(std::to_string(std::chrono::duration_cast<std::chrono::duration<float, std::milli>>(
                                                                end - start)
                                                                .count()) +
                                         "ms")
                                 .green(")")
                                 .build()
                      << std::endl;
            std::cout << std::endl;
            auto core2 = Core::createByBinary(output);
            std::cout << std::endl;
        } catch (const std::exception &e) {
            Exception::printStackTrace(e);
            Profile::clear();
            exit(-1);
        }
    }

}// namespace CHelper::Test