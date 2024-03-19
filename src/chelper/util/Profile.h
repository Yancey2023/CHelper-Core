//
// Created by Yancey on 2024/2/22.
//

#pragma once

#ifndef CHELPER_PROFILE_H
#define CHELPER_PROFILE_H

#include "SimpleLogger.h"

//这个类是为了跟踪代码的运行，在遇到bug的时候方便排查错误的位置
namespace CHelper::Profile {

    extern std::vector<std::string> stack;

    inline void push(const std::string &str) {
        stack.push_back(str);
    }

    inline void pop() {
        stack.pop_back();
    }

    void next(const std::string &str);

    inline void clear() {
        stack.clear();
    }

    std::string getStackTrace();

} // CHelper::Profile

#endif //CHELPER_PROFILE_H
