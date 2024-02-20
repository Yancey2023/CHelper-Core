//
// Created by Yancey666 on 2023/12/1.
//

#include "SimpleLogger.h"

#if CHelperLogger == INFO || CHelperLogger == DEBUG || CHelperLogger == WARN || CHelperLogger == ERROR

namespace CHelper::Logger {

#if CHelperLogger == INFO || CHelperLogger == DEBUG || CHelperLogger == WARN || CHelperLogger == ERROR

    void info(const std::string &message) {
        std::cout << Color::NORMAL << message << std::endl;
    }

    void info(const std::string &key, const std::string &message) {
        std::cout << Color::BLUE << Color::NORMAL << message << std::endl;
    }

#endif

#if CHelperLogger == DEBUG || CHelperLogger == WARN || CHelperLogger == ERROR

    void debug(const std::string &message) {
        std::cout << Color::GREEN << message << std::endl;
    }

    void debug(const std::string &key, const std::string &message) {
        std::cout << Color::BLUE << Color::GREEN << message << std::endl;
    }

#endif

#if CHelperLogger == WARN || CHelperLogger == ERROR

    void warn(const std::string &message) {
        std::cout << Color::YELLOW << message << std::endl;
    }

    void warn(const std::string &key, const std::string &message) {
        std::cout << Color::BLUE << Color::YELLOW << message << std::endl;
    }

#endif

#if CHelperLogger == ERROR

    void error(const std::string &message) {
        std::cout << Color::RED << message << std::endl;
    }

    void error(const std::string &key, const std::string &message) {
        std::cout << Color::BLUE << Color::RED << message << std::endl;
    }

#endif

} // CHelper::Logger

#endif