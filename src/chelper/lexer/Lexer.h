//
// Created by Yancey on 2023/11/6.
//

#ifndef CHELPER_LEXER_H
#define CHELPER_LEXER_H

#include "pch.h"
#include "StringReader.h"
#include "Token.h"

namespace CHelper::Lexer {

    std::vector<Token> lex(StringReader &stringReader);

} // CHelper::Lexer

#endif //CHELPER_LEXER_H
