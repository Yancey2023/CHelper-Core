//
// Created by Yancey on 2023/11/6.
//

#include "Parser.h"
#include "../util/TokenUtil.h"

namespace CHelper::Parser {

    ASTNode parse(TokenReader &&tokenReader, const CPack *cpack) {
        VectorView <Token> tokens = {tokenReader.tokenList, 0, tokenReader.tokenList->size()};
        Profile::push("start parsing: " + TokenUtil::toString(tokens));
        auto mainNode = cpack->mainNode;
        DEBUG_GET_NODE_BEGIN(mainNode)
        auto result = mainNode->getASTNode(tokenReader, cpack);
        DEBUG_GET_NODE_END(mainNode)
        Profile::pop();
        return result;
    }

} // CHelper::Parser