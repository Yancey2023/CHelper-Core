//
// Created by Yancey on 2023/11/5.
//

#include "CHelperCmd.h"
#include "../chelper/Core.h"
#include "param_deliver.h"

int main() {
    //    testDir();
    //    testBin();
    outputSingleJson();
    outputBson();
    outputBinary();
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
    CHelper::Test::testBin(projectDir / "run" / (std::string("beta-experiment-") + CPACK_VERSION_BETA + ".cpack"),
                           projectDir / "test" / "test.txt",
                           true);
}

[[maybe_unused]] void outputSingleJson() {
    std::filesystem::path projectDir(PROJECT_DIR);
    // release
    CHelper::Test::writeSingleJson(projectDir / "resources" / "release" / "vanilla",
                                   projectDir / "run" / "json" / (std::string("release-vanilla-") + CPACK_VERSION_RELEASE + ".json"));
    CHelper::Test::writeSingleJson(projectDir / "resources" / "release" / "experiment",
                                   projectDir / "run" / "json" / (std::string("release-experiment-") + CPACK_VERSION_RELEASE + ".json"));
    // beta
    CHelper::Test::writeSingleJson(projectDir / "resources" / "beta" / "vanilla",
                                   projectDir / "run" / "json" / (std::string("beta-vanilla-") + CPACK_VERSION_BETA + ".json"));
    CHelper::Test::writeSingleJson(projectDir / "resources" / "beta" / "experiment",
                                   projectDir / "run" / "json" / (std::string("beta-experiment-") + CPACK_VERSION_BETA + ".json"));
    // netease
    CHelper::Test::writeSingleJson(projectDir / "resources" / "netease" / "vanilla",
                                   projectDir / "run" / "json" / (std::string("netease-vanilla-") + CPACK_VERSION_NETEASE + ".json"));
    CHelper::Test::writeSingleJson(projectDir / "resources" / "netease" / "experiment",
                                   projectDir / "run" / "json" / (std::string("netease-experiment-") + CPACK_VERSION_NETEASE + ".json"));
}

[[maybe_unused]] void outputBson() {
    std::filesystem::path projectDir(PROJECT_DIR);
    // release
    CHelper::Test::writeBson(projectDir / "resources" / "release" / "vanilla",
                             projectDir / "run" / "bson" / (std::string("release-vanilla-") + CPACK_VERSION_RELEASE + ".bson"));
    CHelper::Test::writeBson(projectDir / "resources" / "release" / "experiment",
                             projectDir / "run" / "bson" / (std::string("release-experiment-") + CPACK_VERSION_RELEASE + ".bson"));
    // beta
    CHelper::Test::writeBson(projectDir / "resources" / "beta" / "vanilla",
                             projectDir / "run" / "bson" / (std::string("beta-vanilla-") + CPACK_VERSION_BETA + ".bson"));
    CHelper::Test::writeBson(projectDir / "resources" / "beta" / "experiment",
                             projectDir / "run" / "bson" / (std::string("beta-experiment-") + CPACK_VERSION_BETA + ".bson"));
    // netease
    CHelper::Test::writeBson(projectDir / "resources" / "netease" / "vanilla",
                             projectDir / "run" / "bson" / (std::string("netease-vanilla-") + CPACK_VERSION_NETEASE + ".bson"));
    CHelper::Test::writeBson(projectDir / "resources" / "netease" / "experiment",
                             projectDir / "run" / "bson" / (std::string("netease-experiment-") + CPACK_VERSION_NETEASE + ".bson"));
}

[[maybe_unused]] void outputBinary() {
    std::filesystem::path projectDir(PROJECT_DIR);
    // release
    CHelper::Test::writeBinary(projectDir / "resources" / "release" / "vanilla",
                               projectDir / "run" / "cpack" / (std::string("release-vanilla-") + CPACK_VERSION_RELEASE + ".cpack"));
    CHelper::Test::writeBinary(projectDir / "resources" / "release" / "experiment",
                               projectDir / "run" / "cpack" / (std::string("release-experiment-") + CPACK_VERSION_RELEASE + ".cpack"));
    // beta
    CHelper::Test::writeBinary(projectDir / "resources" / "beta" / "vanilla",
                               projectDir / "run" / "cpack" / (std::string("beta-vanilla-") + CPACK_VERSION_BETA + ".cpack"));
    CHelper::Test::writeBinary(projectDir / "resources" / "beta" / "experiment",
                               projectDir / "run" / "cpack" / (std::string("beta-experiment-") + CPACK_VERSION_BETA + ".cpack"));
    // netease
    CHelper::Test::writeBinary(projectDir / "resources" / "netease" / "vanilla",
                               projectDir / "run" / "cpack" / (std::string("netease-vanilla-") + CPACK_VERSION_NETEASE + ".cpack"));
    CHelper::Test::writeBinary(projectDir / "resources" / "netease" / "experiment",
                               projectDir / "run" / "cpack" / (std::string("netease-experiment-") + CPACK_VERSION_NETEASE + ".cpack"));
    // old2new
    std::filesystem::path input = projectDir / "resources" / "old2new" / "blockFixData.json";
    std::filesystem::path output = projectDir / "run" / "old2new" / "old2new.dat";
    CHelper::Old2New::BlockFixData blockFixData = CHelper::Old2New::blockFixDataFromJson(CHelper::JsonUtil::getJsonFromFile(input));
    std::filesystem::create_directories(output.parent_path());
    std::ofstream f(output, std::ios::binary);
    CHelper::BinaryWriter binaryWriter(true, f);
    binaryWriter.encode(blockFixData);
    f.close();
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
        CHelper::Test::testDir(cpackPath, commands, isTestTime);
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
                fmt::print("parse successfully({})\n", fmt::styled(std::to_string(std::chrono::duration_cast<std::chrono::duration<float, std::milli>>(endStructure - startParse).count()) + "ms", fg(fmt::color::medium_purple)));
                if (isTestTime) {
                    fmt::print("parse successfully({})\n", fmt::styled(std::to_string(std::chrono::duration_cast<std::chrono::duration<float, std::milli>>(endParse - startParse).count()) + "ms", fg(fmt::color::medium_purple)));
                    fmt::print("get description successfully({})\n", fmt::styled(std::to_string(std::chrono::duration_cast<std::chrono::duration<float, std::milli>>(endDescription - startDescription).count()) + "ms", fg(fmt::color::medium_purple)));
                    fmt::print("get error successfully({})\n", fmt::styled(std::to_string(std::chrono::duration_cast<std::chrono::duration<float, std::milli>>(endErrorReasons - startErrorReasons).count()) + "ms", fg(fmt::color::medium_purple)));
                    fmt::print("get suggestions successfully({})\n", fmt::styled(std::to_string(std::chrono::duration_cast<std::chrono::duration<float, std::milli>>(endSuggestions - startSuggestions).count()) + "ms", fg(fmt::color::medium_purple)));
                    fmt::print("get structure successfully({})\n", fmt::styled(std::to_string(std::chrono::duration_cast<std::chrono::duration<float, std::milli>>(endStructure - startStructure).count()) + "ms", fg(fmt::color::medium_purple)));
                }
#if CHelperTest == true
                std::cout << core->getAstNode()->toJson().dump(-1, ' ', false, nlohmann::detail::error_handler_t::replace) << std::endl;
                std::cout << core->getAstNode()->toBestJson().dump(-1, ' ', false, nlohmann::detail::error_handler_t::replace) << std::endl;
#endif
                std::cout << "structure: " + structure << std::endl;
                std::cout << "description: " + description << std::endl;
                if (errorReasons.empty()) {
                    std::cout << "no error" << std::endl;
                } else {
                    std::cout << "error reasons:" << std::endl;
                    int i = 0;
                    for (const auto &errorReason: errorReasons) {
                        fmt::print("{}. {} {}\n{}{}{}\n",
                                   ++i,
                                   fmt::styled(command.substr(errorReason->start, errorReason->end - errorReason->start), fg(fmt::color::red)),
                                   fmt::styled(errorReason->errorReason, fg(fmt::color::cornflower_blue)),
                                   command.substr(0, errorReason->start),
                                   fmt::styled(errorReason->start == errorReason->end ? "~" : command.substr(errorReason->start, errorReason->end - errorReason->start), fg(fmt::color::red)),
                                   command.substr((errorReason->end)));
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
                        fmt::print("{}. {} {}\n",
                                   ++i,
                                   fmt::styled(item.content->name, fg(fmt::color::lime_green)),
                                   fmt::styled(item.content->description.value_or(""), fg(fmt::color::cornflower_blue)));
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
                        fmt::print("{}{}{}\n",
                                   command.substr(0, item.start),
                                   fmt::styled(greenPart, fg(fmt::color::lime_green)),
                                   command.substr(item.end));
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
            fmt::print(fg(fmt::color::lime_green), "{} commands\n", fmt::styled(commands.size(), fg(fmt::color::medium_purple)));
            fmt::print(fg(fmt::color::lime_green), "parse successfully({})\n", fmt::styled(std::to_string(std::chrono::duration_cast<std::chrono::duration<float, std::milli>>(end - start).count()) + "ms", fg(fmt::color::medium_purple)));
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
            fmt::print(fg(fmt::color::lime_green), "write successfully({})\n", fmt::styled(std::to_string(std::chrono::duration_cast<std::chrono::duration<float, std::milli>>(end - start).count()) + "ms", fg(fmt::color::medium_purple)));
            [[maybe_unused]] auto core2 = Core::createByDirectory(output);
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
            CHELPER_INFO("write successfully({})", std::to_string(std::chrono::duration_cast<std::chrono::duration<float, std::milli>>(end - start).count()) + "ms");
            std::cout << std::endl;
            [[maybe_unused]] auto core2 = Core::createByJson(output);
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
            CHELPER_INFO("write successfully({})", std::to_string(std::chrono::duration_cast<std::chrono::duration<float, std::milli>>(end - start).count()) + "ms");
            std::cout << std::endl;
            [[maybe_unused]] auto core2 = Core::createByBson(output);
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
            CHELPER_INFO("write successfully({})", std::to_string(std::chrono::duration_cast<std::chrono::duration<float, std::milli>>(end - start).count()) + "ms");
            std::cout << std::endl;
            [[maybe_unused]] auto core2 = Core::createByBinary(output);
            std::cout << std::endl;
        } catch (const std::exception &e) {
            Profile::printAndClear(e);
            exit(-1);
        }
    }

}// namespace CHelper::Test

#endif
