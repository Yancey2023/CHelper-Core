//
// Created by Yancey on 2023/11/6.
//

#include "pch.h"

#pragma once

#ifndef CHELPER_POS_H
#define CHELPER_POS_H

namespace CHelper {

    class LexerPos {
    public:
        size_t index;//这个文件的第几个字符
        size_t line; //这个文件的第几行
        size_t col;  //这一行的第几个字符

        explicit LexerPos(size_t index = 0,
                          size_t line = 0,
                          size_t col = 0);

        void next(char ch);
    };

    std::ostream &operator<<(std::ostream &os, const LexerPos &pos);

}// namespace CHelper

#endif//CHELPER_POS_H
