//
// Created by Yancey on 2023/11/6.
//

#include "Parser.h"
#include "../util/TokenUtil.h"
#include "../lexer/Lexer.h"

namespace CHelper::Parser {

    ASTNode parse(const std::string &content, const CPack &cpack) {
        return parse(StringReader(content, "unknown"), cpack);
    }

    ASTNode parse(StringReader &&stringReader, const CPack &cpack) {
        return parse(std::make_shared<std::vector<Token>>(Lexer::lex(stringReader)), cpack);
    }

    ASTNode parse(const std::shared_ptr<std::vector<Token>> &tokens, const CPack &cpack) {
        return parse(TokenReader(tokens), cpack);
    }

    ASTNode parse(TokenReader &&tokenReader, const CPack &cpack) {
        VectorView <Token> tokens = {tokenReader.tokenList, 0, tokenReader.tokenList->size()};
        Profile::push("start parsing: " + TokenUtil::toString(tokens));
        auto result = cpack.mainNode->getASTNode(tokenReader);
        Profile::pop();
        return result;
    }

} // CHelper::Parser