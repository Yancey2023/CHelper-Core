//
// Created by Yancey666 on 2023/11/5.
//

#include "main.h"
#include "chelper/util/StringUtil.h"
#include "chelper/lexer/Lexer.h"
#include "chelper/parser/ASTNode.h"
#include "chelper/util/TokenUtil.h"

#pragma comment(lib, "comctl32.lib")

static size_t ID_INPUT = 1;
static size_t ID_DESCRIPTION = 2;
static size_t ID_LIST_VIEW = 3;

static TCHAR szWindowClass[] = "CHelper";
static TCHAR szTitle[] = "CHelper";

static std::shared_ptr<CHelper::CPack> cpack;

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, PSTR lpCmdLine, int nCmdShow) {
//    CHelper::Test::test(R"(D:\CLion\project\CHelper\test\test.txt)");
//    return 0;
    return initWindows(hInstance, nCmdShow);
}

/**
 * @param hInstance 应用程序的当前实例的句柄
 * @param hPrevInstance 应用程序上一个实例的句柄
 * @param lpCmdLine 应用程序的命令行，不包括程序名称
 * @param nCmdShow 控制窗口的显示方式
 */
int initWindows(HINSTANCE hInstance, int nCmdShow) {
    cpack = std::make_shared<CHelper::CPack>(CHelper::CPack::create(R"(D:\CLion\project\CHelper\resources)"));
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
    if (!RegisterClassEx(&wcex)) {
        MessageBox(nullptr,
                   "Call to RegisterClassEx failed!",
                   "CHelper",
                   NULL);

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
            hInstance, nullptr
    );
    if (!hWnd) {
        MessageBox(nullptr,
                   "Call to CreateWindow failed!",
                   "CHelper",
                   NULL);

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
    return (int) msg.wParam;
}

LRESULT CALLBACK WndProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam) {
    static HWND hWndInput, hWndDescription, hWndListBox;
    switch (uMsg) {
        case WM_CREATE:
            hWndInput = CreateWindow("EDIT", "", WS_CHILD | WS_VISIBLE | WS_BORDER | ES_AUTOHSCROLL,
                                     0, 0, 0, 0, hWnd, (HMENU) ID_INPUT, nullptr, nullptr);
            hWndDescription = CreateWindow("STATIC", "", WS_CHILD | WS_VISIBLE | WS_BORDER | ES_AUTOHSCROLL,
                                           0, 0, 0, 0, hWnd, (HMENU) ID_DESCRIPTION, nullptr, nullptr);
            hWndListBox = CreateWindow(WC_LISTBOX, "", WS_CHILD | WS_VISIBLE | WS_BORDER | WS_VSCROLL | LBS_NOTIFY,
                                       0, 0, 0, 0, hWnd, (HMENU) ID_LIST_VIEW, nullptr, nullptr);
            break;
        case WM_COMMAND:
            if (LOWORD(wParam) == ID_INPUT && HIWORD(wParam) == EN_CHANGE) {
                clock_t start, end;
                start = clock();
                try {
                    int length = GetWindowTextLength(hWndInput);
                    char buffer[length + 1];
                    GetWindowText(hWndInput, buffer, length + 1);
                    std::string command(buffer);
                    std::vector<CHelper::Token> tokens = CHelper::Lexer(command, "unknown").lex();
                    auto parser = CHelper::Parser(CHelper::TokenReader(tokens), *cpack);
                    auto node = parser.parse();
                    std::string description;
                    auto errorReasons = node.getErrorReasons(*cpack);
                    if (errorReasons.empty()) {
                        description = node.getDescription(command.size());
                    } else {
                        if (errorReasons.size() == 1) {
                            const auto &errorReason = errorReasons[0];
                            description = errorReason->errorReason;
                        } else {
                            description.append("错误原因：");
                            int i = 0;
                            for (const auto &item: errorReasons) {
                                description.append("\n").append(std::to_string(i))
                                        .append(". ").append(item->errorReason);
                            }
                        }
                    }
                    {
                        int len = MultiByteToWideChar(CP_UTF8, 0, description.c_str(), -1, nullptr, 0);
                        wchar_t wstr[len];
                        MultiByteToWideChar(CP_UTF8, 0, description.c_str(), -1, wstr, len);
                        SetWindowTextW(hWndDescription, wstr);
                    }
                    auto suggestions = node.getSuggestions(*cpack, command.size());
                    SendMessage(hWndListBox, LB_RESETCONTENT, 0, 0);
                    //由于添加全部结果非常耗时，这里只保留前10个
                    int i = 0;
                    for (const auto &suggestion: suggestions) {
                        if (++i > 10) {
                            break;
                        }
                        auto content = std::string(suggestion.content->name).append(" - ")
                                .append(suggestion.content->description.value_or(""));
                        int len = MultiByteToWideChar(CP_UTF8, 0, content.c_str(), -1, nullptr, 0);
                        wchar_t wstr[len];
                        MultiByteToWideChar(CP_UTF8, 0, content.c_str(), -1, wstr, len);
                        SendMessageW(hWndListBox, LB_ADDSTRING, 0, (LPARAM) wstr);
                    }
                } catch (const std::exception &e) {
                    CHelper::Exception::printStackTrace(e);
                    exit(-1);
                }
                end = clock();
                CHELPER_INFO(CHelper::ColorStringBuilder()
                                     .green("parse load successfully (")
                                     .purple(std::to_string(end - start) + "ms")
                                     .green(")")
                                     .build());
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

namespace CHelper::Test {

    void test(const std::string &testFilePath) {
        std::vector<std::string> commands;
        std::ifstream fin;
        fin.open(testFilePath, std::ios::in);
        std::string cpackPath;
        getline(fin, cpackPath);
        while (fin.is_open()) {
            std::string str;
            getline(fin, str);
            if (str.empty()) {
                break;
            }
            if (str[0] != '-') {
                commands.push_back(str);
            }
        }
        fin.close();
        CHelper::Test::test(cpackPath, commands);
    }

    void test(const std::string &cpackPath, const std::vector<std::string> &commands) {
        try {
            clock_t start, end;
            start = clock();
            auto cpack = getCPack(cpackPath);
            end = clock();
            CHELPER_INFO(ColorStringBuilder()
                                 .green("CPack load successfully (")
                                 .purple(std::to_string(end - start) + "ms")
                                 .green(")")
                                 .build());
            std::cout << std::endl;
            for (const auto &command: commands) {
                start = clock();
                const auto tokens = lex(command);
                auto parser = getParser(cpack, tokens);
                const ASTNode node = parse(parser);
                const auto description = getDescription(node, command.length() - 1);
                const auto errorReasons = getErrorReasons(cpack, node);
                const auto suggestions = getSuggestions(cpack, node, command.length() - 1);
                const auto structure = getStructure(node);
                const auto colors = getColors(node);
                end = clock();
                CHELPER_INFO(ColorStringBuilder()
                                     .green("parse successfully(")
                                     .purple(std::to_string(end - start) + "ms")
                                     .green(")")
                                     .normal(" : ")
                                     .purple(command)
                                     .build());
                CHELPER_INFO("description: " + description);
                if (errorReasons.empty()) {
                    CHELPER_INFO("no error");
                } else if (errorReasons.size() == 1) {
                    const auto &errorReason = errorReasons[0];
                    CHELPER_INFO(ColorStringBuilder()
                                         .normal("error reason: ")
                                         .red(TokenUtil::toString(errorReason->tokens) + " ")
                                         .blue(errorReason->errorReason)
                                         .build());
                } else {
                    CHELPER_INFO("error reasons:");
                    int i = 0;
                    for (const auto &item: errorReasons) {
                        CHELPER_INFO(ColorStringBuilder()
                                             .normal(std::to_string(++i) + ". ")
                                             .red(TokenUtil::toString(item->tokens) + " ")
                                             .blue(item->errorReason)
                                             .build());
                    }
                }
                CHELPER_INFO("suggestions: ");
                int i = 0;
                for (const auto &item: suggestions) {
                    if (i == 10) {
                        CHELPER_INFO("...");
                        break;
                    }
                    CHELPER_INFO(ColorStringBuilder()
                                         .normal(std::to_string(++i) + ". ")
                                         .blue(TokenUtil::toString(item.tokens) + " ")
                                         .yellow(item.content->name + " ")
                                         .normal(item.content->description.value_or(""))
                                         .build());
                }
                std::cout << std::endl;
            }
        } catch (const std::exception &e) {
            Exception::printStackTrace(e);
            exit(-1);
        }
    }

    CPack getCPack(const std::string &source) {
        return CPack::create(source);
    }

    std::vector<Token> lex(const std::string &str) {
        return Lexer(str, "unknown").lex();
    }

    Parser getParser(const CPack &cpack, const std::vector<Token> &tokenList) {
        return {TokenReader(tokenList), cpack};
    }

    ASTNode parse(Parser &parser) {
        return parser.parse();
    }

    std::string getDescription(const ASTNode &node, size_t index) {
        return node.getDescription(index);
    }

    std::vector<std::shared_ptr<ErrorReason>> getErrorReasons(const CPack &cpack, const ASTNode &node) {
        return node.getErrorReasons(cpack);
    }

    std::vector<Suggestion> getSuggestions(const CPack &cpack, const ASTNode &node, size_t index) {
        return node.getSuggestions(cpack, index);
    }

    std::string getStructure(const ASTNode &node) {
        return node.getStructure();
    }

    std::string getColors(const ASTNode &node) {
        return node.getColors();
    }

} // CHelper::Test