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

    void push(const std::string &str);

    void pop();

    void next(const std::string &str);

    void clear();

    void printAndClear(const std::exception &e);

    std::string getStackTrace();

}// namespace CHelper::Profile

#endif//CHELPER_PROFILE_H
