//
// Created by Yancey666 on 2023/11/6.
//

#include "Parser.h"
#include "../util/TokenUtil.h"
#include "../lexer/Lexer.h"

#include <utility>

namespace CHelper {

    Parser::Parser(TokenReader tokenReader, const CPack &cpack)
            : tokenReader(std::move(tokenReader)),
              cpack(cpack),
              mainNode("MAIN_NODE", "main node", cpack.commands) {}

    ASTNode Parser::parse() {
        VectorView <Token> tokens = {&tokenReader.tokenList, 0, tokenReader.tokenList.size()};
        Profile::push("start parsing: " + TokenUtil::toString(tokens));
        auto result = mainNode.getASTNode(tokenReader, cpack);
        Profile::pop();
        return result;
    }

} // CHelper