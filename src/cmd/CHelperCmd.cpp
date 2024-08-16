//
// Created by Yancey on 2023/11/5.
//

#include "CHelperCmd.h"
#include "../chelper/Core.h"
#include "param_deliver.h"
#include <codecvt>
#include <locale>


int main() {
    std::locale::global(std::locale("zh_cn.UTF-8"));
    //    testDir();
    //    testBin();
    outputFile(CHelper::Test::writeSingleJson, L"json");
    outputFile(CHelper::Test::writeBson, L"bson");
    outputFile(CHelper::Test::writeBinary, L"cpack");
    outputOld2New();
    return 0;
}

#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wdeprecated-declarations"

// these two method is slow.
// do not use this implementation in your project.
// you should implement it depend on your platform, such as use Windows API.

static std::wstring_convert<std::codecvt_utf8<wchar_t>> utf8_conv;

std::string wstring2string(const std::wstring &wstring) {
    return utf8_conv.to_bytes(wstring);
}

std::wstring string2wstring(const std::string &string) {
    return utf8_conv.from_bytes(string);
}

#ifdef __clang__
#pragma clang diagnostic pop
#endif

#if CHelperOnlyReadBinary != true

[[maybe_unused]] void testDir() {
    std::filesystem::path projectDir(PROJECT_DIR);
    CHelper::Test::testDir(projectDir / L"resources" / L"beta" / L"vanilla",
                           projectDir / L"test" / L"test.txt",
                           true);
    //    CHelper::Test::testDir(projectDir / L"resources" / L"beta" / L"vanilla",
    //                           std::vector<std::wstring>{"execute run clear "}, false);
}

[[maybe_unused]] void testBin() {
    std::filesystem::path projectDir(PROJECT_DIR);
    CHelper::Test::testBin(projectDir / L"run" / (std::wstring(L"beta-experiment-") + CPACK_VERSION_BETA + L".cpack"),
                           projectDir / L"test" / L"test.txt",
                           true);
}

[[maybe_unused]] void outputFile(
        const std::filesystem::path &projectDir,
        void function(const std::filesystem::path &input, const std::filesystem::path &output),
        const std::wstring &branch1,
        const std::wstring &branch2,
        const std::wstring &version,
        const std::wstring &fileType) {
    std::wstring fileName = branch1 + L'-' + branch2 + L'-' + version + L'.' + fileType;
    CHELPER_INFO(L"----- start output {} -----", fileName);
    function(projectDir / L"resources" / branch1 / branch2,
             projectDir / L"run" / fileType / fileName);
}

[[maybe_unused]] void outputFile(
        void function(const std::filesystem::path &input, const std::filesystem::path &output),
        const std::wstring &fileType) {
    std::filesystem::path projectDir(PROJECT_DIR);
    // release
    outputFile(projectDir, function, L"release", L"vanilla", CPACK_VERSION_RELEASE, fileType);
    outputFile(projectDir, function, L"release", L"experiment", CPACK_VERSION_RELEASE, fileType);
    // beta
    outputFile(projectDir, function, L"beta", L"vanilla", CPACK_VERSION_BETA, fileType);
    outputFile(projectDir, function, L"beta", L"experiment", CPACK_VERSION_BETA, fileType);
    // netease
    outputFile(projectDir, function, L"netease", L"vanilla", CPACK_VERSION_NETEASE, fileType);
    outputFile(projectDir, function, L"netease", L"experiment", CPACK_VERSION_NETEASE, fileType);
}

void outputOld2New() {
    // old2new
    std::filesystem::path projectDir(PROJECT_DIR);
    std::filesystem::path input = projectDir / L"resources" / L"old2new" / L"blockFixData.json";
    std::filesystem::path output = projectDir / L"run" / L"old2new" / L"old2new.dat";
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
    testBin(const std::filesystem::path &cpackPath, const std::vector<std::wstring> &commands, bool isTestTime) {
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
                std::cout << core->getAstNode()->toJson().dump(-1, ' ', false, nlohmann::detail::error_handler_t::replace) << std::endl;
                std::cout << core->getAstNode()->toBestJson().dump(-1, ' ', false, nlohmann::detail::error_handler_t::replace) << std::endl;
#endif
                fmt::print(L"structure: \n", structure);
                fmt::print(L"description: \n", description);
                if (errorReasons.empty()) {
                    std::cout << "no error" << std::endl;
                } else {
                    std::cout << "error reasons:" << std::endl;
                    int i = 0;
                    for (const auto &errorReason: errorReasons) {
                        fmt::print(L"{}. {} {}\n{}{}{}\n",
                                   ++i,
                                   fmt::styled(command.substr(errorReason->start, errorReason->end - errorReason->start), fg(fmt::color::red)),
                                   fmt::styled(errorReason->errorReason, fg(fmt::color::cornflower_blue)),
                                   command.substr(0, errorReason->start),
                                   fmt::styled(errorReason->start == errorReason->end ? L"~" : command.substr(errorReason->start, errorReason->end - errorReason->start), fg(fmt::color::red)),
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
                        fmt::print(L"{}. {} {}\n",
                                   ++i,
                                   fmt::styled(item.content->name, fg(fmt::color::lime_green)),
                                   fmt::styled(item.content->description.value_or(L""), fg(fmt::color::cornflower_blue)));
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
                        fmt::print(L"{}{}{}\n",
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
    [[maybe_unused]] void test2(const std::filesystem::path &cpackPath, const std::vector<std::wstring> &commands, int times) {
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
            fmt::print(L"{}{}",
                       fmt::styled(commands.size(), fg(fmt::color::medium_purple)),
                       fmt::styled(L" commands\n", fg(fmt::color::lime_green)));
            CHELPER_INFO(L"run successfully({})",std::to_wstring(std::chrono::duration_cast<std::chrono::duration<float, std::milli>>(end - start).count()) + L"ms");
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
            CHELPER_INFO(L"CPack write successfully({})",std::to_wstring(std::chrono::duration_cast<std::chrono::duration<float, std::milli>>(end - start).count()) + L"ms");
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
            CHELPER_INFO(L"CPack write successfully({})",std::to_wstring(std::chrono::duration_cast<std::chrono::duration<float, std::milli>>(end - start).count()) + L"ms");
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
            CHELPER_INFO(L"CPack write successfully({})",std::to_wstring(std::chrono::duration_cast<std::chrono::duration<float, std::milli>>(end - start).count()) + L"ms");
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
            CHELPER_INFO(L"CPack write successfully({})",std::to_wstring(std::chrono::duration_cast<std::chrono::duration<float, std::milli>>(end - start).count()) + L"ms");
            [[maybe_unused]] auto core2 = Core::createByBinary(output);
        } catch (const std::exception &e) {
            Profile::printAndClear(e);
            exit(-1);
        }
    }

}// namespace CHelper::Test

#endif
