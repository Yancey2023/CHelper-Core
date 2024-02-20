//
// Created by Yancey666 on 2023/11/6.
//

#include "Parser.h"

#include <utility>

namespace CHelper {

    Parser::Parser(TokenReader tokenReader, const CPack &cpack)
            : tokenReader(std::move(tokenReader)),
              cpack(cpack),
              mainNode("MAIN_NODE", "main node", cpack.commands) {}

    ASTNode Parser::parse() {
        return mainNode.getASTNode(tokenReader, cpack);
    }

} // CHelper