//
// Created by Yancey on 2023/11/5.
//

#include "CHelperCmd.h"
#include "../chelper/Core.h"
#include "param_deliver.h"

int main() {
    //    testDir();
    //    testBin();
    //    outputSingleJson();
    //    outputBson();
    //    outputBinary();
    return 0;
}

#if CHelperWeb != true

[[maybe_unused]] void testDir() {
    std::filesystem::path projectDir(PROJECT_DIR);
    CHelper::Test::testDir(projectDir / "resources" / "beta" / "vanilla",
                           projectDir / "test" / "test.txt",
                           true);
    //    CHelper::Test::testDir(projectDir / "resources" / "beta" / "vanilla",
    //                           std::vector<std::string>{"execute run clear "}, false);
}

[[maybe_unused]] void testBin() {
    std::filesystem::path projectDir(PROJECT_DIR);
    CHelper::Test::testBin(projectDir / "run" / "beta-experiment-1.21.20.21.cpack",
                           projectDir / "test" / "test.txt",
                           true);
}

[[maybe_unused]] void outputSingleJson() {
    std::filesystem::path projectDir(PROJECT_DIR);
    // release
    CHelper::Test::writeSingleJson(projectDir / "resources" / "release" / "vanilla",
                                   projectDir / "run" / "release-vanilla-1.21.1.03.json");
    CHelper::Test::writeSingleJson(projectDir / "resources" / "release" / "experiment",
                                   projectDir / "run" / "release-experiment-1.21.1.03.json");
    // beta
    CHelper::Test::writeSingleJson(projectDir / "resources" / "beta" / "vanilla",
                                   projectDir / "run" / "beta-vanilla-1.21.20.21.json");
    CHelper::Test::writeSingleJson(projectDir / "resources" / "beta" / "experiment",
                                   projectDir / "run" / "beta-experiment-1.21.20.21.json");
    // netease
    CHelper::Test::writeSingleJson(projectDir / "resources" / "netease" / "vanilla",
                                   projectDir / "run" / "netease-vanilla-1.20.10.25.json");
    CHelper::Test::writeSingleJson(projectDir / "resources" / "netease" / "experiment",
                                   projectDir / "run" / "netease-experiment-1.20.10.25.json");
}

[[maybe_unused]] void outputBson() {
    std::filesystem::path projectDir(PROJECT_DIR);
    // release
    CHelper::Test::writeBson(projectDir / "resources" / "release" / "vanilla",
                             projectDir / "run" / "release-vanilla-1.21.1.03.bson");
    CHelper::Test::writeBson(projectDir / "resources" / "release" / "experiment",
                             projectDir / "run" / "release-experiment-1.21.1.03.bson");
    // beta
    CHelper::Test::writeBson(projectDir / "resources" / "beta" / "vanilla",
                             projectDir / "run" / "beta-vanilla-1.21.20.21.bson");
    CHelper::Test::writeBson(projectDir / "resources" / "beta" / "experiment",
                             projectDir / "run" / "beta-experiment-1.21.20.21.bson");
    // netease
    CHelper::Test::writeBson(projectDir / "resources" / "netease" / "vanilla",
                             projectDir / "run" / "netease-vanilla-1.20.10.25.bson");
    CHelper::Test::writeBson(projectDir / "resources" / "netease" / "experiment",
                             projectDir / "run" / "netease-experiment-1.20.10.25.bson");
}

[[maybe_unused]] void outputBinary() {
    std::filesystem::path projectDir(PROJECT_DIR);
    // release
    CHelper::Test::writeBinary(projectDir / "resources" / "release" / "vanilla",
                               projectDir / "run" / "release-vanilla-1.21.1.03.cpack");
    CHelper::Test::writeBinary(projectDir / "resources" / "release" / "experiment",
                               projectDir / "run" / "release-experiment-1.21.1.03.cpack");
    // beta
    CHelper::Test::writeBinary(projectDir / "resources" / "beta" / "vanilla",
                               projectDir / "run" / "beta-vanilla-1.21.20.21.cpack");
    CHelper::Test::writeBinary(projectDir / "resources" / "beta" / "experiment",
                               projectDir / "run" / "beta-experiment-1.21.20.21.cpack");
    // netease
    CHelper::Test::writeBinary(projectDir / "resources" / "netease" / "vanilla",
                               projectDir / "run" / "netease-vanilla-1.20.10.25.cpack");
    CHelper::Test::writeBinary(projectDir / "resources" / "netease" / "experiment",
                               projectDir / "run" / "netease-experiment-1.20.10.25.cpack");
}

namespace CHelper::Test {

    /**
     * 读取测试文件进行测试
     */
    [[maybe_unused]] void testDir(const std::filesystem::path &cpackPath, const std::filesystem::path &testFilePath, bool isTestTime) {
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
        //        CHelper::Test::testDir(cpackPath, commands, isTestTime);
        //        std::vector<std::string> commands1;
        //        for (const auto &item: commands) {
        //            for (size_t i = 0; i < item.size(); i++) {
        //                commands1.push_back(item.substr(0, i + 1));
        //            }
        //        }
        //        CHelper::Test::testDir(cpackPath, commands1, isTestTime);
        //        CHelper::Test::test2(cpackPath, commands1, 10);
    }

    /**
     * 读取测试文件进行测试
     */
    [[maybe_unused]] void testBin(const std::filesystem::path &cpackPath, const std::filesystem::path &testFilePath, bool isTestTime) {
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
        CHelper::Test::testBin(cpackPath, commands, isTestTime);
    }

    /**
     * 测试程序是否可以正常运行
     */
    [[maybe_unused]] void
    testDir(const std::filesystem::path &cpackPath, const std::vector<std::string> &commands, bool isTestTime) {
        Core *core;
        try {
            core = Core::createByDirectory(cpackPath);
            std::cout << std::endl;
            if (HEDLEY_UNLIKELY(core == nullptr)) {
                return;
            }
        } catch (const std::exception &e) {
            Profile::printAndClear(e);
            exit(-1);
        }
        test(core, commands, isTestTime);
    }

    /**
     * 测试程序是否可以正常运行
     */
    [[maybe_unused]] void
    testBin(const std::filesystem::path &cpackPath, const std::vector<std::string> &commands, bool isTestTime) {
        Core *core;
        try {
            core = Core::createByBinary(cpackPath);
            std::cout << std::endl;
            if (HEDLEY_UNLIKELY(core == nullptr)) {
                return;
            }
        } catch (const std::exception &e) {
            Profile::printAndClear(e);
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
                //                std::cout << core->getAstNode()->toJson().dump(-1, ' ', false, nlohmann::detail::error_handler_t::replace)
                //                          << std::endl;
                //                std::cout << core->getAstNode()->toBestJson().dump(-1, ' ', false, nlohmann::detail::error_handler_t::replace)
                //                          << std::endl;
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
            Profile::printAndClear(e);
            exit(-1);
        }
    }

    /**
     * 测试程序性能
     */
    [[maybe_unused]] void test2(const std::filesystem::path &cpackPath, const std::vector<std::string> &commands, int times) {
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
                                 .purple(std::to_string(commands.size()))
                                 .green(" commands")
                                 .build()
                      << std::endl;
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
            Profile::printAndClear(e);
            exit(-1);
        }
    }

    [[maybe_unused]] void writeDirectory(const std::string &input, const std::filesystem::path &output) {
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
            Profile::printAndClear(e);
            exit(-1);
        }
    }

    [[maybe_unused]] void writeSingleJson(const std::filesystem::path &input, const std::filesystem::path &output) {
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
            Profile::printAndClear(e);
            exit(-1);
        }
    }

    [[maybe_unused]] void writeBson(const std::filesystem::path &input, const std::filesystem::path &output) {
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
            Profile::printAndClear(e);
            exit(-1);
        }
    }

    [[maybe_unused]] void writeBinary(const std::filesystem::path &input, const std::filesystem::path &output) {
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
            Profile::printAndClear(e);
            exit(-1);
        }
    }

}// namespace CHelper::Test

#endif
