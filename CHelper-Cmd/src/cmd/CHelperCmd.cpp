//
// Created by Yancey on 2023/11/5.
//

#include "CHelperCmd.h"
#include <chelper/parser/Parser.h>

int main() {
    //    testDir();
    //    testBin();
    outputFile(CHelper::Test::writeSingleJson, "json");
    outputFile(CHelper::Test::writeBinary, "cpack");
    outputOld2New();
    return 0;
}

[[maybe_unused]] void testDir() {
    std::filesystem::path resourceDir(RESOURCE_DIR);
    CHelper::Test::testDir(resourceDir / "resources" / "beta" / "vanilla",
                           resourceDir / "test" / "test.txt",
                           true);
    //    CHelper::Test::testDir(projectDir / "resources" / "beta" / "vanilla",
    //                           std::vector<std::u16string>{"execute run clear "}, false);
}

[[maybe_unused]] void testBin() {
    std::filesystem::path resourceDir(RESOURCE_DIR);
    CHelper::Test::testBin(resourceDir / "run" / (std::string("beta-experiment-") + CPACK_VERSION_BETA + ".cpack"),
                           resourceDir / "test" / "test.txt",
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
    std::filesystem::path projectDir(RESOURCE_DIR);
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
    std::filesystem::path resourceDir(RESOURCE_DIR);
    std::filesystem::path input = resourceDir / "resources" / "old2new" / "blockFixData.json";
    std::filesystem::path output = resourceDir / "run" / "old2new" / "old2new.dat";
    CHelper::Old2New::BlockFixData blockFixData = CHelper::Old2New::blockFixDataFromJson(serialization::get_json_from_file(input));
    std::filesystem::create_directories(output.parent_path());
    std::ofstream ostream(output, std::ios::binary);
    serialization::Codec<decltype(blockFixData)>::to_binary<true>(ostream, blockFixData);
    ostream.close();
}

namespace CHelper::Test {

    /**
     * 读取测试文件进行测试
     */
    [[maybe_unused]] void testDir(const std::filesystem::path &cpackPath, const std::filesystem::path &testFilePath, bool isTestTime) {
        std::vector<std::u16string> commands;
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
            commands.push_back(utf8::utf8to16(str));
        }
        fin.close();
        CHelper::Test::testDir(cpackPath, commands, isTestTime);
        //        std::vector<std::u16string> commands1;
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
        std::vector<std::u16string> commands;
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
            commands.push_back(utf8::utf8to16(str));
        }
        fin.close();
        CHelper::Test::testBin(cpackPath, commands, isTestTime);
    }

    /**
     * 测试程序是否可以正常运行
     */
    [[maybe_unused]] void
    testDir(const std::filesystem::path &cpackPath, const std::vector<std::u16string> &commands, bool isTestTime) {
        CHelperCore *core;
        try {
            core = CHelperCore::createByDirectory(cpackPath);
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
    testBin(const std::filesystem::path &cpackPath, const std::vector<std::u16string> &commands, bool isTestTime) {
        CHelperCore *core;
        try {
            core = CHelperCore::createByBinary(cpackPath);
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
    test(CHelperCore *core, const std::vector<std::u16string> &commands, bool isTestTime) {
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
#ifdef CHelperTest
                fmt::println(core->getAstNode()->toJson().dump(-1, ' ', false, nlohmann::detail::error_handler_t::replace));
                fmt::println(core->getAstNode()->toBestJson().dump(-1, ' ', false, nlohmann::detail::error_handler_t::replace));
#endif
                fmt::print("structure: \n", utf8::utf16to8(structure));
                fmt::print("description: \n", utf8::utf16to8(description));
                if (errorReasons.empty()) {
                    fmt::println("no error");
                } else {
                    fmt::println("error reasons:");
                    int i = 0;
                    for (const auto &errorReason: errorReasons) {
                        fmt::print("{}. {} {}\n{}{}{}\n",
                                   ++i,
                                   fmt::styled(utf8::utf16to8(command.substr(errorReason->start, errorReason->end - errorReason->start)), fg(fmt::color::red)),
                                   fmt::styled(utf8::utf16to8(errorReason->errorReason), fg(fmt::color::cornflower_blue)),
                                   utf8::utf16to8(command.substr(0, errorReason->start)),
                                   fmt::styled(errorReason->start == errorReason->end ? "~" : utf8::utf16to8(command.substr(errorReason->start, errorReason->end - errorReason->start)), fg(fmt::color::red)),
                                   utf8::utf16to8(command.substr((errorReason->end))));
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
                                   fmt::styled(utf8::utf16to8(item.content->name), fg(fmt::color::lime_green)),
                                   fmt::styled(utf8::utf16to8(item.content->description.value_or(u"")), fg(fmt::color::cornflower_blue)));
                        std::u16string result = command.substr(0, item.start)
                                                        .append(item.content->name)
                                                        .append(command.substr(item.end));
                        std::u16string greenPart = item.content->name;
                        if (item.end == command.length()) {
                            ASTNode astNode = Parser::parse(result, core->getCPack());
                            if (item.isAddWhitespace && astNode.isAllWhitespaceError()) {
                                greenPart.push_back(u' ');
                            }
                        }
                        fmt::print("{}{}{}\n",
                                   utf8::utf16to8(command.substr(0, item.start)),
                                   fmt::styled(utf8::utf16to8(greenPart), fg(fmt::color::lime_green)),
                                   utf8::utf16to8(command.substr(item.end)));
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
    [[maybe_unused]] void test2(const std::filesystem::path &cpackPath, const std::vector<std::u16string> &commands, int times) {
        try {
            auto core = CHelperCore::createByDirectory(cpackPath);
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

    [[maybe_unused]] void writeDirectory(const std::u16string &input, const std::filesystem::path &output) {
        try {
            auto core = CHelperCore::createByDirectory(input);
            if (HEDLEY_UNLIKELY(core == nullptr)) {
                return;
            }
            std::chrono::high_resolution_clock::time_point start, end;
            start = std::chrono::high_resolution_clock::now();
            core->getCPack()->writeJsonToDirectory(output);
            end = std::chrono::high_resolution_clock::now();
            CHELPER_INFO("CPack write successfully({})", std::to_string(std::chrono::duration_cast<std::chrono::duration<float, std::milli>>(end - start).count()) + "ms");
            [[maybe_unused]] auto core2 = CHelperCore::createByDirectory(output);
        } catch (const std::exception &e) {
            Profile::printAndClear(e);
            exit(-1);
        }
    }

    [[maybe_unused]] void writeSingleJson(const std::filesystem::path &input, const std::filesystem::path &output) {
        try {
            auto core = CHelperCore::createByDirectory(input);
            if (HEDLEY_UNLIKELY(core == nullptr)) {
                return;
            }
            std::chrono::high_resolution_clock::time_point start, end;
            start = std::chrono::high_resolution_clock::now();
            core->getCPack()->writeJsonToFile(output);
            end = std::chrono::high_resolution_clock::now();
            CHELPER_INFO("CPack write successfully({})", std::to_string(std::chrono::duration_cast<std::chrono::duration<float, std::milli>>(end - start).count()) + "ms");
            [[maybe_unused]] auto core2 = CHelperCore::createByJson(output);
        } catch (const std::exception &e) {
            Profile::printAndClear(e);
            exit(-1);
        }
    }

    [[maybe_unused]] void writeBinary(const std::filesystem::path &input, const std::filesystem::path &output) {
        try {
            auto core = CHelperCore::createByDirectory(input);
            if (HEDLEY_UNLIKELY(core == nullptr)) {
                return;
            }
            std::chrono::high_resolution_clock::time_point start, end;
            start = std::chrono::high_resolution_clock::now();
            core->getCPack()->writeBinToFile(output);
            end = std::chrono::high_resolution_clock::now();
            CHELPER_INFO("CPack write successfully({})", std::to_string(std::chrono::duration_cast<std::chrono::duration<float, std::milli>>(end - start).count()) + "ms");
            [[maybe_unused]] auto core2 = CHelperCore::createByBinary(output);
        } catch (const std::exception &e) {
            Profile::printAndClear(e);
            exit(-1);
        }
    }

}// namespace CHelper::Test

