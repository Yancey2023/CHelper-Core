//
// Created by Yancey666 on 2023/11/6.
//

#include "pch.h"

#ifndef CHELPER_POS_H
#define CHELPER_POS_H

namespace CHelper {

    class LexerPos {
    public:
        size_t line; //这个文件的第几行
        size_t col; //这一行的第几个字符
        size_t index; //这个文件的第几个字符
        const std::string &filePath; //文件相对于functions文件夹的相对路径

        LexerPos(size_t line, size_t col, size_t which, const std::string &fileName);

        void next(char ch);
    };

    std::ostream &operator<<(std::ostream &os, const LexerPos &pos);

}

#endif //CHELPER_POS_H
