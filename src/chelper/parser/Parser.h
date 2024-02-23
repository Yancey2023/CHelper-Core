//
// Created by Yancey on 2023/11/6.
//

#ifndef CHELPER_PARSER_H
#define CHELPER_PARSER_H


#include "pch.h"
#include "ASTNode.h"
#include "../resources/CPack.h"
#include "TokenReader.h"
#include "../node/param/NodeCommand.h"
#include "../lexer/StringReader.h"

namespace CHelper::Parser {

    std::pair<std::shared_ptr<std::vector<Token>>, ASTNode> parse(const std::string &content, const CPack &cpack);

    std::pair<std::shared_ptr<std::vector<Token>>, ASTNode> parse(StringReader &&stringReader, const CPack &cpack);

    ASTNode parse(const std::vector<Token> &tokens, const CPack &cpack);

    ASTNode parse(TokenReader &&tokenReader, const CPack &cpack);

} // CHelper::Parser

#endif //CHELPER_PARSER_H
