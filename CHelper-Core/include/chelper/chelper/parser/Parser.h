//
// Created by Yancey on 2023/11/6.
//

#pragma once

#ifndef CHELPER_PARSER_H
#define CHELPER_PARSER_H

#include "ASTNode.h"
#include "pch.h"

namespace CHelper::Parser {

    ASTNode parse(const std::u16string &content, const CPack *cpack, const Node::NodeBase &mainNode);

    ASTNode parse(const std::u16string &content, const CPack *cpack);

}// namespace CHelper::Parser

#endif//CHELPER_PARSER_H
