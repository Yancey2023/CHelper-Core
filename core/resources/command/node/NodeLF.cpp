//
// Created by Yancey666 on 2023/11/13.
//

#include "NodeLF.h"

namespace CHelper::Node {

    NODE_TYPE_INSTANCE("LF", NodeLF, "line feet")

    ASTNode NodeLF::getASTNode(TokenReader &tokenReader, const CPack &cpack) const {
        tokenReader.push();
        tokenReader.skipAll();
        VectorView<Token> tokens = tokenReader.collect();
        std::shared_ptr<ErrorReason> errorReason;
        if (tokens.hasValue()) {
            errorReason = ErrorReason::excess(tokens, "命令后面有多余部分");
        }
        return ASTNode::simpleNode(this, tokens, errorReason);
    }

} // CHelper::Node