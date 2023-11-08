//
// Created by Yancey666 on 2023/11/6.
//

#include <string>

#ifndef CHELPER_POS_H
#define CHELPER_POS_H

namespace CHelper {

    class Pos {
    public:
        int line; //这个文件的第几行
        int col; //这一行的第几个字符
        int which; //这个文件的第几个字符
        const std::string filePath; //文件相对于functions文件夹的相对路径

        Pos(int line, int col, int which, std::string fileName);

        void next(char ch);
    };

    std::ostream &operator<<(std::ostream &os, const Pos &pos);

}

#endif //CHELPER_POS_H
