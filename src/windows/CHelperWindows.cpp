//
// Created by Yancey on 2024/2/26.
//

#include "CHelperWindows.h"
#include <commctrl.h>
#include "../chelper/Core.h"
#include "../chelper/parser/Parser.h"
#include "param_deliver.h"

static size_t ID_INPUT = 1;
static size_t ID_DESCRIPTION = 2;
static size_t ID_LIST_VIEW = 3;

static TCHAR szWindowClass[] = "CHelper";
static TCHAR szTitle[] = "CHelper";

static CHelper::Core *core = nullptr;

/**
 * @param hInstance 应用程序的当前实例的句柄
 * @param hPrevInstance 应用程序上一个实例的句柄
 * @param lpCmdLine 应用程序的命令行，不包括程序名称
 * @param nCmdShow 控制窗口的显示方式
 */
int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, PSTR lpCmdLine, int nCmdShow) {
    std::filesystem::path projectDir(PROJECT_DIR);
    core = CHelper::Core::createByBinary(projectDir / "run" / "beta-experiment-1.21.20.21.cpack");
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
            onTextChanged("");
            break;
        case WM_COMMAND:
            if (LOWORD(wParam) == ID_INPUT && HIWORD(wParam) == EN_CHANGE) {
                CHelper::Profile::push("get command from input");
                int length = GetWindowTextLength(hWndInput);
                auto *buffer = new char[length + 1];
                GetWindowText(hWndInput, buffer, length + 1);
                UINT codePage = GetACP();
                int wideCharLength = MultiByteToWideChar(codePage, 0, buffer, -1, nullptr, 0);
                auto *wideCharBuffer = new wchar_t[wideCharLength + 1];
                MultiByteToWideChar(codePage, 0, buffer, -1, wideCharBuffer, wideCharLength + 1);
                int utf8Length = WideCharToMultiByte(CP_UTF8, 0, wideCharBuffer, -1, nullptr, 0, nullptr, nullptr);
                char *utf8Buffer = new char[utf8Length + 1];
                WideCharToMultiByte(CP_UTF8, 0, wideCharBuffer, -1, utf8Buffer, utf8Length + 1, nullptr, nullptr);
                CHelper::Profile::pop();
                onTextChanged(utf8Buffer);
                delete[] buffer;
                delete[] wideCharBuffer;
                delete[] utf8Buffer;
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

void onTextChanged(const std::string &command) {
    try {
        std::chrono::high_resolution_clock::time_point startParse, endParse,
                startDescription, endDescription,
                startErrorReasons, endErrorReasons,
                startSuggestions, endSuggestions,
                startStructure, endStructure;
        CHelper::Profile::push(CHelper::ColorStringBuilder()
                                       .red("parsing command: ")
                                       .purple(command)
                                       .build());
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
        CHelper::Profile::push("update description text view");
        {
            int len = MultiByteToWideChar(CP_UTF8, 0, structure.c_str(), -1, nullptr, 0);
            auto *wstr = new wchar_t[len + 1];
            MultiByteToWideChar(CP_UTF8, 0, structure.c_str(), -1, wstr, len);
            SetWindowTextW(hWndDescription, wstr);
            delete[] wstr;
        }
        CHelper::Profile::next("update suggestion list view");
        SendMessage(hWndListBox, LB_RESETCONTENT, 0, 0);
        //由于添加全部结果非常耗时，这里只保留前30个
        int i = 0;
        for (const auto &suggestion: *suggestions) {
            if (++i > 30) {
                break;
            }
            auto content = std::string(suggestion.content->name).append(" - ").append(suggestion.content->description.value_or(""));
            int len = MultiByteToWideChar(CP_UTF8, 0, content.c_str(), -1, nullptr, 0);
            auto *wstr = new wchar_t[len];
            MultiByteToWideChar(CP_UTF8, 0, content.c_str(), -1, wstr, len);
            SendMessageW(hWndListBox, LB_ADDSTRING, 0, (LPARAM) wstr);
            delete[] wstr;
        }
        CHelper::Profile::pop();
        std::cout << CHelper::ColorStringBuilder()
                             .green("parse successfully(")
                             .purple(std::to_string(std::chrono::duration_cast<std::chrono::milliseconds>(
                                                            endStructure - startParse)
                                                            .count()) +
                                     "ms")
                             .green(")")
                             .normal(" : ")
                             .purple(command)
                             .build()
                  << std::endl;
        std::cout << CHelper::ColorStringBuilder().blue("parse in ").purple(std::to_string(std::chrono::duration_cast<std::chrono::duration<float, std::milli>>(endParse - startParse).count()) + "ms").build() << std::endl;
        std::cout << CHelper::ColorStringBuilder().blue("get description in ").purple(std::to_string(std::chrono::duration_cast<std::chrono::duration<float, std::milli>>(endDescription - startDescription).count()) + "ms").build() << std::endl;
        std::cout << CHelper::ColorStringBuilder().blue("get error reasons in ").purple(std::to_string(std::chrono::duration_cast<std::chrono::duration<float, std::milli>>(endErrorReasons - startErrorReasons).count()) + "ms").build() << std::endl;
        std::cout << CHelper::ColorStringBuilder().blue("get suggestions in ").purple(std::to_string(std::chrono::duration_cast<std::chrono::duration<float, std::milli>>(endSuggestions - startSuggestions).count()) + "ms").build() << std::endl;
        std::cout << CHelper::ColorStringBuilder().blue("get structure in ").purple(std::to_string(std::chrono::duration_cast<std::chrono::duration<float, std::milli>>(endStructure - startStructure).count()) + "ms").build() << std::endl;
        //        std::cout << core->getAstNode()->toOptimizedJson().dump(
        //                -1, ' ', false, nlohmann::detail::error_handler_t::replace) << std::endl;
        //        std::cout << core->getAstNode()->toBestJson().dump(
        //                -1, ' ', false, nlohmann::detail::error_handler_t::replace) << std::endl;
        std::cout << "structure: " + structure << std::endl;
        std::cout << "description: " + description << std::endl;
        if (errorReasons.empty()) {
            std::cout << "no error" << std::endl;
        } else {
            std::cout << "error reasons:" << std::endl;
            int i2 = 0;
            for (const auto &errorReason: errorReasons) {
                std::cout << CHelper::ColorStringBuilder()
                                     .normal(std::to_string(++i2) + ". ")
                                     .red(command.substr(errorReason->start,
                                                         errorReason->end - errorReason->start) +
                                          " ")
                                     .blue(errorReason->errorReason)
                                     .build()
                          << std::endl;
                std::cout << CHelper::ColorStringBuilder()
                                     .normal(command.substr(0, errorReason->start))
                                     .red(errorReason->start == errorReason->end ? "~" : command.substr(errorReason->start, errorReason->end - errorReason->start))
                                     .normal(command.substr(errorReason->end))
                                     .build()
                          << std::endl;
            }
        }
        if (suggestions->empty()) {
            std::cout << "no suggestion" << std::endl;
        } else {
            std::cout << "suggestions: " << std::endl;
            int j = 0;
            for (const auto &item: *suggestions) {
                if (j == 30) {
                    std::cout << "..." << std::endl;
                    break;
                }
                std::cout << CHelper::ColorStringBuilder()
                                     .normal(std::to_string(++j) + ". ")
                                     .green(item.content->name + " ")
                                     .blue(item.content->description.value_or(""))
                                     .build()
                          << std::endl;
                std::string result = command.substr(0, item.start)
                                             .append(item.content->name)
                                             .append(command.substr(item.end));
                std::string greenPart = item.content->name;
                if (item.end == command.length()) {
                    CHelper::ASTNode astNode = CHelper::Parser::parse(result, core->getCPack());
                    if (astNode.canAddWhitespace && astNode.isAllWhitespaceError()) {
                        greenPart.push_back(' ');
                    }
                }
                std::cout << CHelper::ColorStringBuilder()
                                     .normal(command.substr(0, item.start))
                                     .green(greenPart)
                                     .normal(command.substr(item.end))
                                     .build()
                          << std::endl;
            }
        }
        std::cout << std::endl;
    } catch (const std::exception &e) {
        std::cout << CHelper::ColorStringBuilder().red("parse failed").build() << std::endl;
        CHelper::Exception::printStackTrace(e);
        CHelper::Profile::clear();
    }
}