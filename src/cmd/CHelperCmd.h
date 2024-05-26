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

namespace CHelper::Test {

    [[maybe_unused]] void testDir(const std::string &cpackPath, const std::string &testFilePath, bool isTestTime);

    [[maybe_unused]] void testBin(const std::string &cpackPath, const std::string &testFilePath, bool isTestTime);

    [[maybe_unused]] void testDir(const std::string &cpackPath, const std::vector<std::string> &commands, bool isTestTime);

    [[maybe_unused]] void testBin(const std::string &cpackPath, const std::vector<std::string> &commands, bool isTestTime);

    [[maybe_unused]] void test(const std::shared_ptr<Core> &core, const std::vector<std::string> &commands, bool isTestTime);

    [[maybe_unused]] void test2(const std::string &cpackPath, const std::vector<std::string> &commands, int times);

    [[maybe_unused]] void writeDirectory(const std::string &input, const std::string &output);

    [[maybe_unused]] void writeSingleJson(const std::string &input, const std::string &output);

    [[maybe_unused]] void writeBson(const std::string &input, const std::string &output);

    [[maybe_unused]] void writeBinary(const std::string &input, const std::string &output);

}// namespace CHelper::Test

#endif//CHELPER_CHELPERCMD_H
