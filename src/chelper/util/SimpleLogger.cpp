//
// Created by Yancey on 2023/12/1.
//

#include "SimpleLogger.h"

#if CHelperLogger == INFO || CHelperLogger == DEBUG || CHelperLogger == WARN || CHelperLogger == ERROR

namespace CHelper::Logger {

#if CHelperLogger == INFO || CHelperLogger == DEBUG || CHelperLogger == WARN || CHelperLogger == ERROR

    void info(const std::string &message) {
#if CHelperAndroid == true
        __android_log_print(ANDROID_LOG_INFO, "NativeCHelper", "%s", message.c_str());
#else
        std::cout << Color::NORMAL << message << std::endl;
#endif
    }

    void info(const std::string &key, const std::string &message) {
#if CHelperAndroid == true
        __android_log_print(ANDROID_LOG_INFO, "NativeCHelper", "%s", message.c_str());
#else
        std::cout << Color::NORMAL << Color::NORMAL << message << std::endl;
#endif
    }

#endif

#if CHelperLogger == DEBUG || CHelperLogger == WARN || CHelperLogger == ERROR

    void debug(const std::string &message) {
#if CHelperAndroid == true
        __android_log_print(ANDROID_LOG_DEBUG, "NativeCHelper", "%s", message.c_str());
#else
        std::cout << Color::GREEN << message << std::endl;
#endif
    }

    void debug(const std::string &key, const std::string &message) {
#if CHelperAndroid == true
        __android_log_print(ANDROID_LOG_DEBUG, "NativeCHelper", "%s", message.c_str());
#else
        std::cout << Color::GREEN << Color::GREEN << message << std::endl;
#endif
    }

#endif

#if CHelperLogger == WARN || CHelperLogger == ERROR

    void warn(const std::string &message) {
#if CHelperAndroid == true
        __android_log_print(ANDROID_LOG_WARN, "NativeCHelper", "%s", message.c_str());
#else
        std::cout << Color::YELLOW << message << std::endl;
#endif
    }

    void warn(const std::string &key, const std::string &message) {
#if CHelperAndroid == true
        __android_log_print(ANDROID_LOG_WARN, "NativeCHelper", "%s", message.c_str());
#else
        std::cout << Color::YELLOW << Color::YELLOW << message << std::endl;
#endif
    }

#endif

#if CHelperLogger == ERROR

    void error(const std::string &message) {
#if CHelperAndroid == true
        __android_log_print(ANDROID_LOG_ERROR, "NativeCHelper", "%s", message.c_str());
#else
        std::cout << Color::RED << message << std::endl;
#endif
    }

    void error(const std::string &key, const std::string &message) {
#if CHelperAndroid == true
        __android_log_print(ANDROID_LOG_ERROR, "NativeCHelper", "%s", message.c_str());
#else
        std::cout << Color::RED << Color::RED << message << std::endl;
#endif
    }

#endif

} // CHelper::Logger

#endif