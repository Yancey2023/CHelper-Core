//
// Created by Yancey on 2023/12/1.
//

#include "SimpleLogger.h"

#if CHelperLogger == DEBUG || CHelperLogger == INFO || CHelperLogger == WARN || CHelperLogger == ERROR

namespace CHelper::Logger {

#if CHelperAndroid == true
    const char* KEY = "NativeCHelper";
#endif

    void debug(const std::string &message) {
#if CHelperAndroid == true
        __android_log_print(ANDROID_LOG_DEBUG, KEY, "%s", message.c_str());
#else
        std::cout << ColorStringBuilder().green(message).build() << std::endl;
#endif
    }

#if CHelperLogger == INFO || CHelperLogger == WARN || CHelperLogger == ERROR

    void info(const std::string &message) {
#if CHelperAndroid == true
        __android_log_print(ANDROID_LOG_INFO, KEY, "%s", message.c_str());
#else
        std::cout << ColorStringBuilder().normal(message).build() << std::endl;
#endif
    }

#endif

#if CHelperLogger == WARN || CHelperLogger == ERROR

    void warn(const std::string &message) {
#if CHelperAndroid == true
        __android_log_print(ANDROID_LOG_WARN, KEY, "%s", message.c_str());
#else
        std::cout << ColorStringBuilder().yellow(message).build() << std::endl;
#endif
    }

#endif

#if CHelperLogger == ERROR

    void error(const std::string &message) {
#if CHelperAndroid == true
        __android_log_print(ANDROID_LOG_ERROR, KEY, "%s", message.c_str());
#else
        std::cout << ColorStringBuilder().red(message).build() << std::endl;
#endif
    }

#endif

} // CHelper::Logger

#endif