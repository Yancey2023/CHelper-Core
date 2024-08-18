//
// Created by Yancey on 2024/2/26.
//

#include "CHelperWindows.h"
#include "../chelper/Core.h"
#include "../chelper/parser/Parser.h"
#include "param_deliver.h"
#include <AtlBase.h>
#include <atlconv.h>
#include <commctrl.h>

static size_t ID_INPUT = 1;
static size_t ID_DESCRIPTION = 2;
static size_t ID_LIST_VIEW = 3;

static TCHAR szWindowClass[] = "CHelper";
static TCHAR szTitle[] = "CHelper";

static CHelper::Core *core = nullptr;

std::string wstring2string(const std::wstring &wstring) {
    CW2A ca2a(wstring.c_str(), CP_UTF8);
    std::string result = ca2a.m_szBuffer;
    return result;
}

std::wstring string2wstring(const std::string &string) {
    CA2W ca2w(string.c_str(), CP_UTF8);
    std::wstring result = ca2w.m_szBuffer;
    return result;
}

/**
 * @param hInstance 应用程序的当前实例的句柄
 * @param hPrevInstance 应用程序上一个实例的句柄
 * @param lpCmdLine 应用程序的命令行，不包括程序名称
 * @param nCmdShow 控制窗口的显示方式
 */
int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, PSTR lpCmdLine, int nCmdShow) {
    std::locale::global(std::locale("zh_cn.UTF-8"));
    std::filesystem::path projectDir(PROJECT_DIR);
    core = CHelper::Core::createByBinary(projectDir / L"run" / L"cpack" / (std::wstring(L"beta-experiment-") + CPACK_VERSION_BETA + L".cpack"));
    if (HEDLEY_UNLIKELY(core == nullptr)) {
        exit(-1);
    }
    //窗口数据
    WNDCLASSEX wcex;
    wcex.cbWndExtra = 0;
    wcex.cbClsExtra = 0;
    wcex.cbSize = sizeof(WNDCLASSEX);
    wcex.style = CS_HREDRAW | CS_VREDRAW;
    wcex.lpfnWndProc = WndProc;
    wcex.hInstance = hInstance;
    wcex.hIcon = LoadIcon(wcex.hInstance, IDI_APPLICATION);
    wcex.hCursor = LoadCursor(nullptr, IDC_ARROW);
    wcex.hbrBackground = (HBRUSH) (COLOR_WINDOW + 1);
    wcex.lpszMenuName = nullptr;
    wcex.lpszClassName = szWindowClass;
    wcex.hIconSm = LoadIcon(wcex.hInstance, IDI_APPLICATION);
    //注册窗口
    if (HEDLEY_UNLIKELY(!RegisterClassEx(&wcex))) {
        MessageBox(nullptr, "Call to RegisterClassEx failed!", "CHelper", 0);
        return 1;
    }
    //创建窗口
    HWND hWnd = CreateWindowEx(
            WS_EX_OVERLAPPEDWINDOW,
            szWindowClass,
            szTitle,
            WS_OVERLAPPEDWINDOW,
            CW_USEDEFAULT, CW_USEDEFAULT,
            500, 500,
            nullptr, nullptr,
            hInstance, nullptr);
    if (HEDLEY_UNLIKELY(hWnd == nullptr)) {
        MessageBox(nullptr, "Call to CreateWindow failed!", "CHelper", 0);
        return 1;
    }
    //显示并更新窗口
    ShowWindow(hWnd, nCmdShow);
    UpdateWindow(hWnd);
    //循环
    MSG msg;
    while (GetMessage(&msg, nullptr, 0, 0)) {
        TranslateMessage(&msg);
        DispatchMessage(&msg);
    }
    delete core;
    core = nullptr;
    return (int) msg.wParam;
}

static HWND hWndInput, hWndDescription, hWndListBox;

LRESULT CALLBACK WndProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam) {
    switch (uMsg) {
        case WM_CREATE:
            hWndInput = CreateWindow("EDIT", "", WS_CHILD | WS_VISIBLE | WS_BORDER | ES_AUTOHSCROLL,
                                     0, 0, 0, 0, hWnd, (HMENU) ID_INPUT, nullptr, nullptr);
            hWndDescription = CreateWindow("STATIC", "", WS_CHILD | WS_VISIBLE | WS_BORDER | ES_AUTOHSCROLL,
                                           0, 0, 0, 0, hWnd, (HMENU) ID_DESCRIPTION, nullptr, nullptr);
            hWndListBox = CreateWindow(WC_LISTBOX, "", WS_CHILD | WS_VISIBLE | WS_BORDER | WS_VSCROLL | LBS_NOTIFY,
                                       0, 0, 0, 0, hWnd, (HMENU) ID_LIST_VIEW, nullptr, nullptr);
            onTextChanged(L"");
            break;
        case WM_COMMAND:
            if (LOWORD(wParam) == ID_INPUT && HIWORD(wParam) == EN_CHANGE) {
                CHelper::Profile::push(L"get command from input");
                int length = GetWindowTextLength(hWndInput);
                auto *buffer = new char[length + 1];
                GetWindowText(hWndInput, buffer, length + 1);
                UINT codePage = GetACP();
                int wideCharLength = MultiByteToWideChar(codePage, 0, buffer, -1, nullptr, 0);
                auto *wideCharBuffer = new wchar_t[wideCharLength + 1];
                MultiByteToWideChar(codePage, 0, buffer, -1, wideCharBuffer, wideCharLength + 1);
                CHelper::Profile::pop();
                onTextChanged(wideCharBuffer);
                delete[] buffer;
                delete[] wideCharBuffer;
            }
            break;
        case WM_SIZE:
            MoveWindow(hWndInput, 10, HIWORD(lParam) - 30, LOWORD(lParam) - 20, 20, TRUE);
            MoveWindow(hWndDescription, 10, 10, LOWORD(lParam) - 20, 20, TRUE);
            MoveWindow(hWndListBox, 10, 40, LOWORD(lParam) - 20, HIWORD(lParam) - 70, TRUE);
            break;
        case WM_CLOSE:
            DestroyWindow(hWnd);
            break;
        case WM_DESTROY:
            PostQuitMessage(0);
            break;
        default:
            return DefWindowProc(hWnd, uMsg, wParam, lParam);
    }
    return 0;
}

void onTextChanged(const std::wstring &command) {
    try {
        std::chrono::high_resolution_clock::time_point startParse, endParse,
                startDescription, endDescription,
                startErrorReasons, endErrorReasons,
                startSuggestions, endSuggestions,
                startStructure, endStructure;
        CHelper::Profile::push(L"parsing command: {}", command);
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
        CHelper::Profile::push(L"update description text view");
        SetWindowTextW(hWndDescription, structure.c_str());
        CHelper::Profile::next(L"update suggestion list view");
        SendMessage(hWndListBox, LB_RESETCONTENT, 0, 0);
        //由于添加全部结果非常耗时，这里只保留前30个
        {
            int i = 0;
            for (const auto &suggestion: *suggestions) {
                if (++i > 30) {
                    break;
                }
                auto content = std::wstring(suggestion.content->name).append(L" - ").append(suggestion.content->description.value_or(L""));
                CW2W cw2w(content.c_str(), CP_UTF8);
                SendMessageW(hWndListBox, LB_ADDSTRING, 0, reinterpret_cast<LPARAM>(cw2w.m_szBuffer));
            }
        }
        CHelper::Profile::pop();
        fmt::print("parse successfully({})\n", fmt::styled(std::to_string(std::chrono::duration_cast<std::chrono::duration<float, std::milli>>(endStructure - startParse).count()) + "ms", fg(fmt::color::medium_purple)));
        fmt::print("parse successfully({})\n", fmt::styled(std::to_string(std::chrono::duration_cast<std::chrono::duration<float, std::milli>>(endParse - startParse).count()) + "ms", fg(fmt::color::medium_purple)));
        fmt::print("get description successfully({})\n", fmt::styled(std::to_string(std::chrono::duration_cast<std::chrono::duration<float, std::milli>>(endDescription - startDescription).count()) + "ms", fg(fmt::color::medium_purple)));
        fmt::print("get error successfully({})\n", fmt::styled(std::to_string(std::chrono::duration_cast<std::chrono::duration<float, std::milli>>(endErrorReasons - startErrorReasons).count()) + "ms", fg(fmt::color::medium_purple)));
        fmt::print("get suggestions successfully({})\n", fmt::styled(std::to_string(std::chrono::duration_cast<std::chrono::duration<float, std::milli>>(endSuggestions - startSuggestions).count()) + "ms", fg(fmt::color::medium_purple)));
        fmt::print("get structure successfully({})\n", fmt::styled(std::to_string(std::chrono::duration_cast<std::chrono::duration<float, std::milli>>(endStructure - startStructure).count()) + "ms", fg(fmt::color::medium_purple)));
#if CHelperTest == true
        std::cout << core->getAstNode()->toOptimizedJson().dump(-1, ' ', false, nlohmann::detail::error_handler_t::replace) << std::endl;
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
                    CHelper::ASTNode astNode = CHelper::Parser::parse(result, core->getCPack());
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
    } catch (const std::exception &e) {
        CHELPER_ERROR("parse failed");
        CHelper::Profile::printAndClear(e);
    }
}