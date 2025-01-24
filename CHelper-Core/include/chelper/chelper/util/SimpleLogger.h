//
// Created by Yancey on 2023/12/1.
//

#pragma once

#ifndef CHELPER_SIMPLELOGGER_H
#define CHELPER_SIMPLELOGGER_H

#include "pch.h"

namespace CHelper::Logger {

    template<typename T>
    auto convertArg(const T &arg) {
        if constexpr (std::is_same<T, std::u16string>() ||
                      std::is_same<T, const std::u16string>() ||
                      std::is_same<T, char16_t *>() ||
                      std::is_same<T, const char16_t *>()) {
            return utf8::utf16to8(static_cast<const std::u16string &>(arg));
        } else {
            return arg;
        }
    }

    template<typename T>
    auto convertArg(const T &&arg) {
        if constexpr (std::is_same<T, std::u16string>() ||
                      std::is_same<T, const std::u16string>()) {
            return utf8::utf16to8(static_cast<const std::u16string &>(arg));
        } else if constexpr (std::is_same<T, char16_t *>() ||
                             std::is_same<T, const char16_t *>()) {
            return utf8::utf16to8(arg);
        } else {
            return arg;
        }
    }

#ifdef CHelperAndroid
    static const char *KEY = "CHelperNative";
#endif

#if CHelperLogger == DEBUG
    template<typename... T>
    void debug(const std::string &fmt, T &&...args) {
#ifdef CHelperAndroid
        std::string content = fmt::format(fmt, convertArg(args)...);
        __android_log_print(ANDROID_LOG_DEBUG, KEY, "%s", content.c_str());
#else
        fmt::print("{}\t{}\n",
                   fmt::styled("[DEBUG]", fg(fmt::color::lime_green)),
                   fmt::format(fmt, fmt::styled(convertArg(args), fg(fmt::color::medium_purple))...));
        fflush(stdout);
#endif
    }
#endif

#if CHelperLogger == DEBUG || CHelperLogger == INFO
    template<typename... T>
    void info(const std::string &fmt, T &&...args) {
#ifdef CHelperAndroid
        std::string content = fmt::format(fmt, convertArg(args)...);
        __android_log_print(ANDROID_LOG_INFO, KEY, "%s", content.c_str());
#else
        fmt::print("{}\t{}\n",
                   fmt::styled("[INFO]", fg(fmt::color::cornflower_blue)),
                   fmt::format(fmt, fmt::styled(convertArg(args), fg(fmt::color::medium_purple))...));
        fflush(stdout);
#endif
    }
#endif

#if CHelperLogger == DEBUG || CHelperLogger == INFO || CHelperLogger == WARN
    template<typename... T>
    void warn(const std::string &fmt, T &&...args) {
#ifdef CHelperAndroid
        std::string content = fmt::format(fmt, convertArg(args)...);
        __android_log_print(ANDROID_LOG_WARN, KEY, "%s", content.c_str());
#else
        fmt::print("{}\t{}\n",
                   fmt::styled("[WARN]", fg(fmt::color::gold)),
                   fmt::format(fmt, fmt::styled(convertArg(args), fg(fmt::color::medium_purple))...));
        fflush(stdout);
#endif
    }
#endif

#if CHelperLogger == DEBUG || CHelperLogger == INFO || CHelperLogger == WARN || CHelperLogger == ERROR
    template<typename... T>
    void error(const std::string &fmt, T &&...args) {
#ifdef CHelperAndroid
        std::string content = fmt::format(fmt, convertArg(args)...);
        __android_log_print(ANDROID_LOG_ERROR, KEY, "%s", content.c_str());
#else
        fmt::print("{}\t{}\n",
                   fmt::styled("[ERROR]", fg(fmt::color::red)),
                   fmt::format(fmt, fmt::styled(convertArg(args), fg(fmt::color::medium_purple))...));
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
