//
// Created by Yancey on 2023/11/6.
//

#pragma once

#ifndef CHELPER_PARSER_H
#define CHELPER_PARSER_H


#include "../lexer/Lexer.h"
#include "../lexer/StringReader.h"
#include "../node/param/NodeCommand.h"
#include "../resources/CPack.h"
#include "ASTNode.h"
#include "TokenReader.h"
#include "pch.h"

namespace CHelper::Parser {

    ASTNode parse(TokenReader &&tokenReader, const CPack *cpack);

    ASTNode parse(const std::shared_ptr<std::vector<Token>> &tokens, const CPack *cpack);

    ASTNode parse(StringReader stringReader, const CPack *cpack);

    ASTNode parse(const std::string &content, const CPack *cpack);

}// namespace CHelper::Parser

#endif//CHELPER_PARSER_H
