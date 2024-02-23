//
// Created by Yancey on 2023/11/6.
//

#ifndef CHELPER_MAIN_H
#define CHELPER_MAIN_H

#include "pch.h"
#include "chelper/resources/CPack.h"
#include "chelper/parser/Parser.h"
#include "chelper/parser/Suggestion.h"

int initWindows(HINSTANCE hInstance, int nCmdShow);

LRESULT CALLBACK WndProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam);

void onTextChanged(const std::string& command);

namespace CHelper::Test {

    void test(const std::string &testFilePath);

    void test(const std::string &cpackPath, const std::vector<std::string> &commands);

} // CHelper::Test

#endif //CHELPER_MAIN_H
