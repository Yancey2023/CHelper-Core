//
// Created by Yancey on 2023/11/5.
//

#include "CHelperCmd.h"
#include "../chelper/Core.h"
#include <codecvt>
#include <locale>

int main() {
    //    testDir();
    //    testBin();
    outputFile(CHelper::Test::writeSingleJson, "json");
    outputFile(CHelper::Test::writeBson, "bson");
    outputFile(CHelper::Test::writeBinary, "cpack");
    outputOld2New();
    return 0;
}

#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wdeprecated-declarations"

// these two method is slow and codecvt is deprecated in c++17
// do not use this implementation in your project.
// you should implement it depend on your platform, such as use Windows API.

static std::wstring_convert<std::codecvt_utf8<wchar_t>> utf8_conv;

std::string wstring2string(const std::wstring &wstring) {
    return utf8_conv.to_bytes(wstring);
}

std::wstring string2wstring(const std::string &string) {
    return utf8_conv.from_bytes(string);
}

#pragma clang diagnostic pop

#if CHelperOnlyReadBinary != true

[[maybe_unused]] void testDir() {
    std::filesystem::path projectDir(PROJECT_DIR);
    CHelper::Test::testDir(projectDir / "resources" / "beta" / "vanilla",
                           projectDir / "test" / "test.txt",
                           true);
    //    CHelper::Test::testDir(projectDir / "resources" / "beta" / "vanilla",
    //                           std::vector<std::wstring>{"execute run clear "}, false);
}

[[maybe_unused]] void testBin() {
    std::filesystem::path projectDir(PROJECT_DIR);
    CHelper::Test::testBin(projectDir / "run" / (std::string("beta-experiment-") + CPACK_VERSION_BETA + ".cpack"),
                           projectDir / "test" / "test.txt",
                           true);
}

[[maybe_unused]] void outputFile(
        const std::filesystem::path &projectDir,
        void function(const std::filesystem::path &input, const std::filesystem::path &output),
        const std::string &branch1,
        const std::string &branch2,
        const std::string &version,
        const std::string &fileType) {
    std::string fileName = branch1 + '-' + branch2 + '-' + version + '.' + fileType;
    CHELPER_INFO("----- start output {} -----", fileName);
    function(projectDir / "resources" / branch1 / branch2,
             projectDir / "run" / fileType / fileName);
}

[[maybe_unused]] void outputFile(
        void function(const std::filesystem::path &input, const std::filesystem::path &output),
        const std::string &fileType) {
    std::filesystem::path projectDir(PROJECT_DIR);
    // release
    outputFile(projectDir, function, "release", "vanilla", CPACK_VERSION_RELEASE, fileType);
    outputFile(projectDir, function, "release", "experiment", CPACK_VERSION_RELEASE, fileType);
    // beta
    outputFile(projectDir, function, "beta", "vanilla", CPACK_VERSION_BETA, fileType);
    outputFile(projectDir, function, "beta", "experiment", CPACK_VERSION_BETA, fileType);
    // netease
    outputFile(projectDir, function, "netease", "vanilla", CPACK_VERSION_NETEASE, fileType);
    outputFile(projectDir, function, "netease", "experiment", CPACK_VERSION_NETEASE, fileType);
}

void outputOld2New() {
    // old2new
    std::filesystem::path projectDir(PROJECT_DIR);
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
        std::vector<std::wstring> commands;
        std::wifstream fin;
        fin.open(testFilePath, std::ios::in);
        while (fin.is_open()) {
            std::wstring str;
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
        //        std::vector<std::wstring> commands1;
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
        std::vector<std::wstring> commands;
        std::wifstream fin;
        fin.open(testFilePath, std::ios::in);
        while (fin.is_open()) {
            std::wstring str;
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
    testDir(const std::filesystem::path &cpackPath, const std::vector<std::wstring> &commands, bool isTestTime) {
        Core *core;
        try {
            core = Core::createByDirectory(cpackPath);
            fmt::print("\n");
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
    testBin(const std::filesystem::path &cpackPath, const std::vector<std::wstring> &commands, bool isTestTime) {
        Core *core;
        try {
            core = Core::createByBinary(cpackPath);
            fmt::print("\n");
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
    test(Core *core, const std::vector<std::wstring> &commands, bool isTestTime) {
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
                fmt::println(core->getAstNode()->toJson().dump(-1, ' ', false, nlohmann::detail::error_handler_t::replace));
                fmt::println(core->getAstNode()->toBestJson().dump(-1, ' ', false, nlohmann::detail::error_handler_t::replace));
#endif
                fmt::print("structure: \n", wstring2string(structure));
                fmt::print("description: \n", wstring2string(description));
                if (errorReasons.empty()) {
                    fmt::println("no error");
                } else {
                    fmt::println("error reasons:");
                    int i = 0;
                    for (const auto &errorReason: errorReasons) {
                        fmt::print("{}. {} {}\n{}{}{}\n",
                                   ++i,
                                   fmt::styled(wstring2string(command.substr(errorReason->start, errorReason->end - errorReason->start)), fg(fmt::color::red)),
                                   fmt::styled(wstring2string(errorReason->errorReason), fg(fmt::color::cornflower_blue)),
                                   wstring2string(command.substr(0, errorReason->start)),
                                   fmt::styled(errorReason->start == errorReason->end ? "~" : wstring2string(command.substr(errorReason->start, errorReason->end - errorReason->start)), fg(fmt::color::red)),
                                   wstring2string(command.substr((errorReason->end))));
                    }
                }
                if (suggestions->empty()) {
                    fmt::println("no suggestion");
                } else {
                    fmt::println("suggestions: ");
                    int i = 0;
                    for (const auto &item: *suggestions) {
                        if (i == 30) {
                            fmt::println("...");
                            break;
                        }
                        fmt::print("{}. {} {}\n",
                                   ++i,
                                   fmt::styled(wstring2string(item.content->name), fg(fmt::color::lime_green)),
                                   fmt::styled(wstring2string(item.content->description.value_or(L"")), fg(fmt::color::cornflower_blue)));
                        std::wstring result = command.substr(0, item.start)
                                                      .append(item.content->name)
                                                      .append(command.substr(item.end));
                        std::wstring greenPart = item.content->name;
                        if (item.end == command.length()) {
                            ASTNode astNode = Parser::parse(result, core->getCPack());
                            if (item.isAddWhitespace && astNode.isAllWhitespaceError()) {
                                greenPart.push_back(L' ');
                            }
                        }
                        fmt::print("{}{}{}\n",
                                   wstring2string(command.substr(0, item.start)),
                                   fmt::styled(wstring2string(greenPart), fg(fmt::color::lime_green)),
                                   wstring2string(command.substr(item.end)));
                    }
                }
                fmt::print("\n");
            }
        } catch (const std::exception &e) {
            Profile::printAndClear(e);
            exit(-1);
        }
    }

    /**
     * 测试程序性能
     */
    [[maybe_unused]] void test2(const std::filesystem::path &cpackPath, const std::vector<std::wstring> &commands, int times) {
        try {
            auto core = Core::createByDirectory(cpackPath);
            fmt::print("\n");
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
            fmt::print("{}{}",
                       fmt::styled(commands.size(), fg(fmt::color::medium_purple)),
                       fmt::styled(" commands\n", fg(fmt::color::lime_green)));
            CHELPER_INFO("run successfully({})", std::to_string(std::chrono::duration_cast<std::chrono::duration<float, std::milli>>(end - start).count()) + "ms");
        } catch (const std::exception &e) {
            Profile::printAndClear(e);
            exit(-1);
        }
    }

    [[maybe_unused]] void writeDirectory(const std::wstring &input, const std::filesystem::path &output) {
        try {
            auto core = Core::createByDirectory(input);
            if (HEDLEY_UNLIKELY(core == nullptr)) {
                return;
            }
            std::chrono::high_resolution_clock::time_point start, end;
            start = std::chrono::high_resolution_clock::now();
            core->getCPack()->writeJsonToDirectory(output);
            end = std::chrono::high_resolution_clock::now();
            CHELPER_INFO("CPack write successfully({})", std::to_string(std::chrono::duration_cast<std::chrono::duration<float, std::milli>>(end - start).count()) + "ms");
            [[maybe_unused]] auto core2 = Core::createByDirectory(output);
        } catch (const std::exception &e) {
            Profile::printAndClear(e);
            exit(-1);
        }
    }

    [[maybe_unused]] void writeSingleJson(const std::filesystem::path &input, const std::filesystem::path &output) {
        try {
            auto core = Core::createByDirectory(input);
            if (HEDLEY_UNLIKELY(core == nullptr)) {
                return;
            }
            std::chrono::high_resolution_clock::time_point start, end;
            start = std::chrono::high_resolution_clock::now();
            core->getCPack()->writeJsonToFile(output);
            end = std::chrono::high_resolution_clock::now();
            CHELPER_INFO("CPack write successfully({})", std::to_string(std::chrono::duration_cast<std::chrono::duration<float, std::milli>>(end - start).count()) + "ms");
            [[maybe_unused]] auto core2 = Core::createByJson(output);
        } catch (const std::exception &e) {
            Profile::printAndClear(e);
            exit(-1);
        }
    }

    [[maybe_unused]] void writeBson(const std::filesystem::path &input, const std::filesystem::path &output) {
        try {
            auto core = Core::createByDirectory(input);
            if (HEDLEY_UNLIKELY(core == nullptr)) {
                return;
            }
            std::chrono::high_resolution_clock::time_point start, end;
            start = std::chrono::high_resolution_clock::now();
            core->getCPack()->writeBsonToFile(output);
            end = std::chrono::high_resolution_clock::now();
            CHELPER_INFO("CPack write successfully({})", std::to_string(std::chrono::duration_cast<std::chrono::duration<float, std::milli>>(end - start).count()) + "ms");
        } catch (const std::exception &e) {
            Profile::printAndClear(e);
            exit(-1);
        }
    }

    [[maybe_unused]] void writeBinary(const std::filesystem::path &input, const std::filesystem::path &output) {
        try {
            auto core = Core::createByDirectory(input);
            if (HEDLEY_UNLIKELY(core == nullptr)) {
                return;
            }
            std::chrono::high_resolution_clock::time_point start, end;
            start = std::chrono::high_resolution_clock::now();
            core->getCPack()->writeBinToFile(output);
            end = std::chrono::high_resolution_clock::now();
            CHELPER_INFO("CPack write successfully({})", std::to_string(std::chrono::duration_cast<std::chrono::duration<float, std::milli>>(end - start).count()) + "ms");
            [[maybe_unused]] auto core2 = Core::createByBinary(output);
        } catch (const std::exception &e) {
            Profile::printAndClear(e);
            exit(-1);
        }
    }

}// namespace CHelper::Test

#endif
