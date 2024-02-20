//
// Created by Yancey666 on 2023/11/6.
//

#include "pch.h"

#ifndef CHELPER_POS_H
#define CHELPER_POS_H

namespace CHelper {

    class LexerPos {
    public:
        int line; //这个文件的第几行
        int col; //这一行的第几个字符
        int index; //这个文件的第几个字符
        const std::string &filePath; //文件相对于functions文件夹的相对路径

        LexerPos(int line, int col, int which, const std::string &fileName);

        void next(char ch);
    };

    std::ostream &operator<<(std::ostream &os, const LexerPos &pos);

}

#endif //CHELPER_POS_H
