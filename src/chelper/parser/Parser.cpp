//
// Created by Yancey on 2023/11/6.
//

#include "Parser.h"
#include "../util/TokenUtil.h"

namespace CHelper::Parser {

    ASTNode parse(TokenReader &&tokenReader, const CPack *cpack) {
        VectorView<Token> tokens = {tokenReader.tokenList, 0, tokenReader.tokenList->size()};
        Profile::push("start parsing: " + TokenUtil::toString(tokens));
        auto mainNode = cpack->mainNode.get();
        DEBUG_GET_NODE_BEGIN(mainNode)
        auto result = mainNode->getASTNode(tokenReader, cpack);
        DEBUG_GET_NODE_END(mainNode)
        Profile::pop();
        return result;
    }

    ASTNode parse(const std::shared_ptr<std::vector<Token>> &tokens, const CPack *cpack) {
        return parse(TokenReader(tokens), cpack);
    }

    ASTNode parse(StringReader stringReader, const CPack *cpack) {
        return parse(std::make_shared<std::vector<Token>>(Lexer::lex(stringReader)), cpack);
    }

    ASTNode parse(const std::string &content, const CPack *cpack) {
        return parse(StringReader(content, "unknown"), cpack);
    }

}// namespace CHelper::Parser