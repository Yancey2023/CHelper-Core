//
// Created by Yancey on 2023/11/13.
//

#include "NodeLF.h"

namespace CHelper::Node {

    NODE_TYPE_INSTANCE("LF", NodeLF, "line feet")

    ASTNode NodeLF::getASTNode(TokenReader &tokenReader, const CPack &cpack) const {
        tokenReader.push();
        tokenReader.skipToLF();
        VectorView <Token> tokens = tokenReader.collect();
        std::shared_ptr<ErrorReason> errorReason;
        if (tokens.hasValue()) {
            errorReason = ErrorReason::excess(tokens, "命令后面有多余部分");
        }
        return ASTNode::simpleNode(this, tokens, errorReason);
    }

    void NodeLF::collectStructure(const ASTNode *astNode,
                                  StructureBuilder &structure,
                                  bool isMustHave) const {
        structure.append("\n");
    }

} // CHelper::Node