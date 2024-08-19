//
// Created by Yancey on 2023/11/6.
//

#pragma once

#ifndef CHELPER_CHELPERCMD_H
#define CHELPER_CHELPERCMD_H

#include "../chelper/parser/Parser.h"
#include "../chelper/parser/Suggestion.h"
#include "../chelper/resources/CPack.h"
#include "pch.h"

int main();

#if CHelperOnlyReadBinary != true

[[maybe_unused]] void testDir();

[[maybe_unused]] void testBin();

[[maybe_unused]] void outputFile(
        const std::filesystem::path &projectDir,
        void function(const std::filesystem::path &input, const std::filesystem::path &output),
        const std::string &str1,
        const std::string &str2,
        const std::string &str3,
        const std::string &fileType);

[[maybe_unused]] void outputFile(
        void function(const std::filesystem::path &input, const std::filesystem::path &output),
        const std::string &fileType);

[[maybe_unused]] void outputOld2New();

namespace CHelper::Test {

    [[maybe_unused]] void testDir(const std::filesystem::path &cpackPath, const std::filesystem::path &testFilePath, bool isTestTime);

    [[maybe_unused]] void testBin(const std::filesystem::path &cpackPath, const std::filesystem::path &testFilePath, bool isTestTime);

    [[maybe_unused]] void testDir(const std::filesystem::path &cpackPath, const std::vector<std::wstring> &commands, bool isTestTime);

    [[maybe_unused]] void testBin(const std::filesystem::path &cpackPath, const std::vector<std::wstring> &commands, bool isTestTime);

    [[maybe_unused]] void test(Core *core, const std::vector<std::wstring> &commands, bool isTestTime);

    [[maybe_unused]] void test2(const std::filesystem::path &cpackPath, const std::vector<std::wstring> &commands, int times);

    [[maybe_unused]] void writeDirectory(const std::filesystem::path &input, const std::filesystem::path &output);

    [[maybe_unused]] void writeSingleJson(const std::filesystem::path &input, const std::filesystem::path &output);

    [[maybe_unused]] void writeBson(const std::filesystem::path &input, const std::filesystem::path &output);

    [[maybe_unused]] void writeBinary(const std::filesystem::path &input, const std::filesystem::path &output);

}// namespace CHelper::Test

#endif

#endif//CHELPER_CHELPERCMD_H
