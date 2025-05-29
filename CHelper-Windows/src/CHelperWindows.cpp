//
// Created by Yancey on 2024/2/26.
//

#include "CHelperWindows.h"
#include <chelper/ChelperCore.h>
#include <chelper/parser/Parser.h>
#include <commctrl.h>

static size_t ID_INPUT = 1;
static size_t ID_DESCRIPTION = 2;
static size_t ID_LIST_VIEW = 3;

static TCHAR szWindowClass[] = TEXT("CHelper");
static TCHAR szTitle[] = TEXT("CHelper");

static CHelper::CHelperCore *core = nullptr;

/**
 * @param hInstance 应用程序的当前实例的句柄
 * @param hPrevInstance 应用程序上一个实例的句柄
 * @param lpCmdLine 应用程序的命令行，不包括程序名称
 * @param nCmdShow 控制窗口的显示方式
 */
int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, PSTR lpCmdLine, int nCmdShow) {
    std::filesystem::path resourceDir(RESOURCE_DIR);
    for (const auto &cpackPath: std::filesystem::directory_iterator(resourceDir / "run" / "cpack")) {
        std::string fileName = cpackPath.path().filename().string();
        if (fileName.find("beta-experiment-") != -1) {
            core = CHelper::CHelperCore::createByBinary(cpackPath);
            break;
        }
    }
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
    wcex.hbrBackground = reinterpret_cast<HBRUSH>(COLOR_WINDOW + 1);
    wcex.lpszMenuName = nullptr;
    wcex.lpszClassName = szWindowClass;
    wcex.hIconSm = LoadIcon(wcex.hInstance, IDI_APPLICATION);
    //注册窗口
    if (HEDLEY_UNLIKELY(!RegisterClassEx(&wcex))) {
        MessageBox(nullptr, TEXT("Call to RegisterClassEx failed!"), TEXT("CHelper"), 0);
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
        MessageBox(nullptr, TEXT("Call to CreateWindow failed!"), TEXT("CHelper"), 0);
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
    return static_cast<int>(msg.wParam);
}

static HWND hWndInput, hWndDescription, hWndListBox;

LRESULT CALLBACK WndProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam) {
    switch (uMsg) {
        case WM_CREATE:
            hWndInput = CreateWindow(TEXT("EDIT"), TEXT(""), WS_CHILD | WS_VISIBLE | WS_BORDER | ES_AUTOHSCROLL,
                                     0, 0, 0, 0, hWnd, reinterpret_cast<HMENU>(ID_INPUT), nullptr, nullptr);
            hWndDescription = CreateWindow(TEXT("STATIC"), TEXT(""), WS_CHILD | WS_VISIBLE | WS_BORDER | ES_AUTOHSCROLL,
                                           0, 0, 0, 0, hWnd, reinterpret_cast<HMENU>(ID_DESCRIPTION), nullptr, nullptr);
            hWndListBox = CreateWindow(WC_LISTBOX, TEXT("LIST"), WS_CHILD | WS_VISIBLE | WS_BORDER | WS_VSCROLL | LBS_NOTIFY,
                                       0, 0, 0, 0, hWnd, reinterpret_cast<HMENU>(ID_LIST_VIEW), nullptr, nullptr);
            onTextChanged(u"");
            break;
        case WM_COMMAND:
            if (LOWORD(wParam) == ID_INPUT && HIWORD(wParam) == EN_CHANGE) {
                CHelper::Profile::push("get command from input");
                int length = GetWindowTextLength(hWndInput);
                auto *buffer = new WCHAR[length + 1];
                GetWindowText(hWndInput, buffer, length + 1);
                onTextChanged(reinterpret_cast<char16_t *>(buffer));
                delete[] buffer;
            } else if (LOWORD(wParam) == ID_LIST_VIEW && HIWORD(wParam) == LBN_SELCHANGE) {
                int index = SendMessage(hWndListBox, LB_GETCURSEL, 0, 0);
                std::optional<std::pair<std::u16string, size_t>> result = core->onSuggestionClick(index);
                if (result.has_value()) {
                    SetWindowText(hWndInput, reinterpret_cast<const WCHAR *>(result.value().first.c_str()));
                    SendMessage(hWndInput, EM_SETSEL, result.value().second, -1);
                }
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

void onTextChanged(const std::u16string &command) {
    try {
        std::chrono::high_resolution_clock::time_point startParse, endParse,
                startDescription, endDescription,
                startErrorReasons, endErrorReasons,
                startSuggestions, endSuggestions,
                startStructure, endStructure;
        CHelper::Profile::push("parsing command: {}", command);
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
            auto structureUtf8 = utf8::utf16to8(structure);
            int len = MultiByteToWideChar(CP_UTF8, 0, structureUtf8.c_str(), -1, nullptr, 0);
            auto *wstr = new wchar_t[len + 1];
            MultiByteToWideChar(CP_UTF8, 0, structureUtf8.c_str(), -1, wstr, len);
            SetWindowTextW(hWndDescription, wstr);
            delete[] wstr;
        }
        CHelper::Profile::next("update suggestion list view");
        SendMessage(hWndListBox, LB_RESETCONTENT, 0, 0);
        //由于添加全部结果非常耗时，这里只保留前30个
        {
            int i = 0;
            for (const auto &suggestion: *suggestions) {
                if (++i > 30) {
                    break;
                }
                auto content = std::u16string(suggestion.content->name)
                                       .append(u" - ")
                                       .append(suggestion.content->description.value_or(u""));
                SendMessage(hWndListBox, LB_ADDSTRING, 0, reinterpret_cast<LPARAM>(content.c_str()));
            }
        }
        CHelper::Profile::pop();
        fmt::print("parse successfully({})\n", fmt::styled(std::to_string(std::chrono::duration_cast<std::chrono::duration<float, std::milli>>(endStructure - startParse).count()) + "ms", fg(fmt::color::medium_purple)));
        fmt::print("parse successfully({})\n", fmt::styled(std::to_string(std::chrono::duration_cast<std::chrono::duration<float, std::milli>>(endParse - startParse).count()) + "ms", fg(fmt::color::medium_purple)));
        fmt::print("get description successfully({})\n", fmt::styled(std::to_string(std::chrono::duration_cast<std::chrono::duration<float, std::milli>>(endDescription - startDescription).count()) + "ms", fg(fmt::color::medium_purple)));
        fmt::print("get error successfully({})\n", fmt::styled(std::to_string(std::chrono::duration_cast<std::chrono::duration<float, std::milli>>(endErrorReasons - startErrorReasons).count()) + "ms", fg(fmt::color::medium_purple)));
        fmt::print("get suggestions successfully({})\n", fmt::styled(std::to_string(std::chrono::duration_cast<std::chrono::duration<float, std::milli>>(endSuggestions - startSuggestions).count()) + "ms", fg(fmt::color::medium_purple)));
        fmt::print("get structure successfully({})\n", fmt::styled(std::to_string(std::chrono::duration_cast<std::chrono::duration<float, std::milli>>(endStructure - startStructure).count()) + "ms", fg(fmt::color::medium_purple)));
#ifdef CHelperTest
        fmt::println(core->getAstNode()->toJson().dump(-1, ' ', false, nlohmann::detail::error_handler_t::replace));
        fmt::println(core->getAstNode()->toBestJson().dump(-1, ' ', false, nlohmann::detail::error_handler_t::replace));
#endif
        fmt::print("structure: \n", utf8::utf16to8(structure));
        fmt::print("description: \n", utf8::utf16to8(description));
        if (errorReasons.empty()) {
            fmt::println("no error");
        } else {
            fmt::println("error reasons:");
            int i = 0;
            for (const auto &errorReason: errorReasons) {
                fmt::print("{}. {} {}\n{}{}{}\n",
                           ++i,
                           fmt::styled(utf8::utf16to8(command.substr(errorReason->start, errorReason->end - errorReason->start)), fg(fmt::color::red)),
                           fmt::styled(utf8::utf16to8(errorReason->errorReason), fg(fmt::color::cornflower_blue)),
                           utf8::utf16to8(command.substr(0, errorReason->start)),
                           fmt::styled(errorReason->start == errorReason->end ? "~" : utf8::utf16to8(command.substr(errorReason->start, errorReason->end - errorReason->start)), fg(fmt::color::red)),
                           utf8::utf16to8(command.substr((errorReason->end))));
            }
        }
        if (suggestions->empty()) {
            fmt::println("no suggestion");
        } else {
            fmt::println("suggestions: ");
            int i = 0;
            for (const auto &item: *suggestions) {
                if (i == 30) {
                    fmt::println("...");
                    break;
                }
                fmt::print("{}. {} {}\n",
                           ++i,
                           fmt::styled(utf8::utf16to8(item.content->name), fg(fmt::color::lime_green)),
                           fmt::styled(utf8::utf16to8(item.content->description.value_or(u"")), fg(fmt::color::cornflower_blue)));
                std::u16string result = command.substr(0, item.start)
                                                .append(item.content->name)
                                                .append(command.substr(item.end));
                std::u16string greenPart = item.content->name;
                if (item.end == command.length()) {
                    CHelper::ASTNode astNode = CHelper::Parser::parse(result, core->getCPack());
                    if (item.isAddWhitespace && astNode.isAllWhitespaceError()) {
                        greenPart.push_back(u' ');
                    }
                }
                fmt::print("{}{}{}\n",
                           utf8::utf16to8(command.substr(0, item.start)),
                           fmt::styled(utf8::utf16to8(greenPart), fg(fmt::color::lime_green)),
                           utf8::utf16to8(command.substr(item.end)));
            }
        }
        fmt::print("\n");
    } catch (const std::exception &e) {
        CHELPER_ERROR("parse failed");
        CHelper::Profile::printAndClear(e);
    }
}