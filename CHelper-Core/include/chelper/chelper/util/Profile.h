//
// Created by Yancey on 2024/2/22.
//

#pragma once

#ifndef CHELPER_PROFILE_H
#define CHELPER_PROFILE_H

#include "SimpleLogger.h"

/**
 * 跟踪代码的运行，为了在遇到bug的时候方便排查错误的位置
 */
namespace CHelper::Profile {

    extern std::vector<std::string> stack;

    template<typename... T>
    void push(const std::string &fmt, T &&...args) {
#ifdef CHelperAndroid
        stack.push_back(fmt::format(fmt, Logger::convertArg(args)...));
#else
        stack.push_back(fmt::format(fmt, styled(Logger::convertArg(args), fg(fmt::color::medium_purple))...));
#endif
    }

    void pop();

    template<typename... T>
    void next(const std::string &fmt, T &&...args) {
        pop();
        push(fmt, args...);
    }

    void clear();

    void printAndClear(const std::exception &e);

    std::string getStackTrace();

}// namespace CHelper::Profile

#endif//CHELPER_PROFILE_H
