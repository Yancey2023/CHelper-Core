//
// Created by Yancey on 2024/2/22.
//

#pragma once

#ifndef CHELPER_PROFILE_H
#define CHELPER_PROFILE_H

/**
 * 跟踪代码的运行，为了在遇到bug的时候方便排查错误的位置
 */
namespace CHelper::Profile {

    extern std::vector<std::string> stack;

    template<typename... T>
    void push(fmt::format_string<T...> fmt, T &&...args) {
        stack.push_back(fmt::format(fmt, args...));
    }

    void pop();

    template<typename... T>
    void next(const std::string &fmt, T &&...args) {
        pop();
        push(fmt, args...);
    }

    void clear();

    void printAndClear(const std::exception &e);

}// namespace CHelper::Profile

#endif//CHELPER_PROFILE_H
