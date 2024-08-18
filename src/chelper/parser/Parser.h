//
// Created by Yancey on 2023/11/6.
//

#pragma once

#ifndef CHELPER_PARSER_H
#define CHELPER_PARSER_H

#include "../lexer/Lexer.h"
#include "../lexer/StringReader.h"
#include "../lexer/TokenReader.h"
#include "../node/param/NodeCommand.h"
#include "../resources/CPack.h"
#include "ASTNode.h"
#include "pch.h"

namespace CHelper::Parser {

    ASTNode parse(const std::wstring &content, const CPack *cpack, const Node::NodeBase *mainNode);

    ASTNode parse(const std::wstring &content, const CPack *cpack);

}// namespace CHelper::Parser

#endif//CHELPER_PARSER_H
