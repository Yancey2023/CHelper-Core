//
// Created by Yancey666 on 2023/11/6.
//

#ifndef CHELPER_MAIN_H
#define CHELPER_MAIN_H

#include "pch.h"
#include "core/lexer/Lexer.h"
#include "core/resources/CPack.h"

void test();

void testLexer(const std::string& str);

void testJsonCPack(const std::string &source);

CHelper::CPack getCPack(const std::string &source);

void testParser(const CHelper::CPack &cpack, const std::string &command);

#endif //CHELPER_MAIN_H
