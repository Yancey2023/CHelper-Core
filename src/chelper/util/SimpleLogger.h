//
// Created by Yancey666 on 2023/12/1.
//

#ifndef CHELPER_SIMPLELOGGER_H
#define CHELPER_SIMPLELOGGER_H

#include "pch.h"

namespace CHelper::Logger {

#if CHelperLogger == INFO || CHelperLogger == DEBUG || CHelperLogger == WARN || CHelperLogger == ERROR

    void info(const std::string &message);

    void info(const std::string &key, const std::string &message);

#endif

#if CHelperLogger == DEBUG || CHelperLogger == WARN || CHelperLogger == ERROR

    void debug(const std::string &message);

    void debug(const std::string &key, const std::string &message);

#endif

#if CHelperLogger == WARN || CHelperLogger == ERROR

    void warn(const std::string &message);

    void warn(const std::string &key, const std::string &message);

#endif

#if CHelperLogger == ERROR

    void error(const std::string &message);

    void error(const std::string &key, const std::string &message);

#endif

} // CHelper::Logger

#if CHELPER_LOGGER == INFO || CHelperLogger == DEBUG || CHelperLogger == WARN || CHelperLogger == ERROR
#define CHELPER_INFO(message) CHelper::Logger::info(message)
#else
#define CHELPER_LOG(message)
#endif

#if CHelperLogger == DEBUG || CHelperLogger == WARN || CHelperLogger == ERROR
#define CHELPER_DEBUG(message) CHelper::Logger::debug(message)
#else
#define CHELPER_DEBUG(message)
#endif

#if CHelperLogger == WARN || CHelperLogger == ERROR
#define CHELPER_WARN(message) CHelper::Logger::warn(message)
#else
#define CHELPER_WARN(message)
#endif

#if CHelperLogger == ERROR
#define CHELPER_ERROR(message) CHelper::Logger::error(message)
#else
#define CHELPER_ERROR(message)
#endif

#endif //CHELPER_SIMPLELOGGER_H
