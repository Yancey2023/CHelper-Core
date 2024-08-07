//
// Created by Yancey on 2023/12/1.
//

#pragma once

#ifndef CHELPER_SIMPLELOGGER_H
#define CHELPER_SIMPLELOGGER_H

#include "fmt/color.h"
#include "fmt/format.h"

namespace CHelper::Logger {

#if CHelperAndroid == true
    static const char *KEY = "NativeCHelper";
#endif

#if CHelperLogger == DEBUG
    template<typename... T>
    void debug(fmt::format_string<T...> fmt, T &&...args) {
#if CHelperAndroid == true
        __android_log_print(ANDROID_LOG_DEBUG, KEY, "%s", format(fmt, args));
#else
        fmt::print("{}\t{}\n",
                   fmt::styled("[DEBUG]", fg(fmt::color::lime_green)),
                   fmt::format(fmt, fmt::styled(args, fg(fmt::color::medium_purple))...));
        fflush(stdout);
#endif
    }
#endif

#if CHelperLogger == DEBUG || CHelperLogger == INFO
    template<typename... T>
    void info(fmt::format_string<T...> fmt, T &&...args) {
#if CHelperAndroid == true
        __android_log_print(ANDROID_LOG_INFO, KEY, "%s", format(fmt, args));
#else
        fmt::print("{}\t{}\n",
                   fmt::styled("[INFO]", fg(fmt::color::cornflower_blue)),
                   fmt::format(fmt, fmt::styled(args, fg(fmt::color::medium_purple))...));
        fflush(stdout);
#endif
    }
#endif

#if CHelperLogger == DEBUG || CHelperLogger == INFO || CHelperLogger == WARN
    template<typename... T>
    void warn(fmt::format_string<T...> fmt, T &&...args) {
#if CHelperAndroid == true
        __android_log_print(ANDROID_LOG_WARN, KEY, "%s", format(fmt, args));
#else
        fmt::print("{}\t{}\n",
                   fmt::styled("[WARN]", fg(fmt::color::gold)),
                   fmt::format(fmt, fmt::styled(args, fg(fmt::color::medium_purple))...));
        fflush(stdout);
#endif
    }
#endif

#if CHelperLogger == DEBUG || CHelperLogger == INFO || CHelperLogger == WARN || CHelperLogger == ERROR
    template<typename... T>
    void error(fmt::format_string<T...> fmt, T &&...args) {
#if CHelperAndroid == true
        __android_log_print(ANDROID_LOG_ERROR, KEY, "%s", format(fmt, args));
#else
        fmt::print("{}\t{}\n",
                   fmt::styled("[ERROR]", fg(fmt::color::red)),
                   fmt::format(fmt, fmt::styled(args, fg(fmt::color::medium_purple))...));
        fflush(stdout);
#endif
    }
#endif

}// namespace CHelper::Logger

#if CHelperLogger == DEBUG
#define CHELPER_DEBUG(...) CHelper::Logger::debug(__VA_ARGS__)
#else
#define CHELPER_DEBUG(...)
#endif

#if CHelperLogger == DEBUG || CHelperLogger == INFO
#define CHELPER_INFO(...) CHelper::Logger::info(__VA_ARGS__)
#else
#define CHELPER_INFO(...)
#endif

#if CHelperLogger == DEBUG || CHelperLogger == INFO || CHelperLogger == WARN
#define CHELPER_WARN(...) CHelper::Logger::warn(__VA_ARGS__)
#else
#define CHELPER_WARN(...)
#endif

#if CHelperLogger == DEBUG || CHelperLogger == INFO || CHelperLogger == WARN || CHelperLogger == ERROR
#define CHELPER_ERROR(...) CHelper::Logger::error(__VA_ARGS__)
#else
#define CHELPER_ERROR(...)
#endif

#endif//CHELPER_SIMPLELOGGER_H
