//
// Created by Yancey666 on 2023/11/6.
//

#ifndef CHELPER_MAIN_H
#define CHELPER_MAIN_H

#include "pch.h"
#include "chelper/resources/CPack.h"
#include "chelper/parser/Parser.h"
#include "chelper/parser/Suggestion.h"

int initWindows(HINSTANCE hInstance, int nCmdShow);

LRESULT CALLBACK WndProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam);

namespace CHelper::Test {

    void test(const std::string &testFilePath);

    void test(const std::string &cpackPath, const std::vector<std::string> &commands);

    CPack getCPack(const std::string &source);

    std::vector<Token> lex(const std::string &str);

    Parser getParser(const CPack &cpack, const std::vector<Token> &tokenList);

    ASTNode parse(Parser &parser);

    std::string getDescription(const ASTNode &node, size_t index);

    std::vector<std::shared_ptr<ErrorReason>> getErrorReasons(const CPack &cpack, const ASTNode &node);

    std::vector<Suggestion> getSuggestions(const CPack &cpack, const ASTNode &node, size_t index);

    std::string getStructure(const ASTNode &node);

    std::string getColors(const ASTNode &node);

} // CHelper::Test

#endif //CHELPER_MAIN_H