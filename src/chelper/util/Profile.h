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

    extern std::vector<std::wstring> stack;

    template<typename... T>
    void push(fmt::wformat_string<T...> fmt, T &&...args) {
#ifdef CHelperAndroid
        stack.push_back(fmt::format(fmt, args...));
#else
        stack.push_back(fmt::format(fmt, styled(args, fg(fmt::color::medium_purple))...));
#endif
    }

    void pop();

    template<typename... T>
    void next(fmt::wformat_string<T...> fmt, T &&...args) {
        pop();
        push(fmt, args...);
    }

    void clear();

    void printAndClear(const std::exception &e);

    std::wstring getStackTrace();

}// namespace CHelper::Profile

#endif//CHELPER_PROFILE_H
