//
// Created by Yancey on 2024/2/26.
//

#pragma once

#ifndef CHELPER_CHELPERWINDOWS_H
#define CHELPER_CHELPERWINDOWS_H

#define UNICODE

#include "pch.h"
#include <windows.h>

#pragma comment(lib, "comctl32.lib")

LRESULT CALLBACK WndProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam);

void onTextChanged(const std::u16string &command);

#endif//CHELPER_CHELPERWINDOWS_H
