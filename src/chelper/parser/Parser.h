//
// Created by Yancey on 2023/11/6.
//

#pragma once

#ifndef CHELPER_PARSER_H
#define CHELPER_PARSER_H


#include "pch.h"
#include "ASTNode.h"
#include "../resources/CPack.h"
#include "TokenReader.h"
#include "../node/param/NodeCommand.h"
#include "../lexer/StringReader.h"
#include "../lexer/Lexer.h"

namespace CHelper::Parser {

    ASTNode parse(TokenReader &&tokenReader, const CPack *cpack);

    inline ASTNode parse(const std::shared_ptr<std::vector<Token>> &tokens, const CPack *cpack) {
        return parse(TokenReader(tokens), cpack);
    }

    inline ASTNode parse(StringReader stringReader, const CPack *cpack) {
        return parse(std::make_shared<std::vector<Token>>(Lexer::lex(stringReader)), cpack);
    }

    inline ASTNode parse(const std::string &content, const CPack *cpack) {
        return parse(StringReader(content, "unknown"), cpack);
    }

} // CHelper::Parser

#endif //CHELPER_PARSER_H
