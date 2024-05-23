//
// Created by Yancey on 2023/11/6.
//

#pragma once

#ifndef CHELPER_LEXER_H
#define CHELPER_LEXER_H

#include "StringReader.h"
#include "Token.h"
#include "pch.h"

namespace CHelper::Lexer {

    std::vector<Token> lex(StringReader stringReader);

}// namespace CHelper::Lexer

#endif//CHELPER_LEXER_H
