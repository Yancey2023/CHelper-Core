//
// Created by Yancey666 on 2023/11/6.
//

#ifndef CHELPER_PARSER_H
#define CHELPER_PARSER_H


#include <vector>
#include "../lexer/Token.h"

class Parser {
    std::vector<Token> tokenList;

public:
    explicit Parser(const std::vector<Token> &tokenList);
};


#endif //CHELPER_PARSER_H
