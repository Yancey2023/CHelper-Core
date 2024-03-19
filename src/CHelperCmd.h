//
// Created by Yancey on 2023/11/6.
//

#pragma once

#ifndef CHELPER_CHELPERCMD_H
#define CHELPER_CHELPERCMD_H

#include "pch.h"
#include "chelper/resources/CPack.h"
#include "chelper/parser/Parser.h"
#include "chelper/parser/Suggestion.h"

int main();

namespace CHelper::Test {

    [[maybe_unused]] void test(const std::string &cpackPath, const std::string &testFilePath, bool isTestTime);

    [[maybe_unused]] void test(const std::string &cpackPath, const std::vector<std::string> &commands, bool isTestTime);

    [[maybe_unused]] void test2(const std::string &cpackPath, const std::vector<std::string> &commands, int times);

    [[maybe_unused]] void test3(const std::string &input, const std::string &output);

    [[maybe_unused]] void test4(const std::string &input, const std::string &output);

    [[maybe_unused]] void test5(const std::string &input, const std::string &output);

} // CHelper::Test

#endif //CHELPER_CHELPERCMD_H
