//
// Created by Yancey on 2023/11/6.
//

#ifndef CHELPER_CHELPERCMD_H
#define CHELPER_CHELPERCMD_H

#include "pch.h"
#include "chelper/resources/CPack.h"
#include "chelper/parser/Parser.h"
#include "chelper/parser/Suggestion.h"

int main();

namespace CHelper::Test {

    void test(const std::string &cpackPath, const std::string &testFilePath, bool isTestTime);

    void test(const std::string &cpackPath, const std::vector<std::string> &commands, bool isTestTime);

    void test2(const std::string &cpackPath, const std::vector<std::string> &commands, int times);

} // CHelper::Test

#endif //CHELPER_CHELPERCMD_H
