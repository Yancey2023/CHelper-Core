//
// Created by Yancey666 on 2023/12/18.
//

#include "NodeBlockName.h"

namespace CHelper::Node {

    NODE_TYPE_INSTANCE("BLOCK_NAME", NodeBlockName, "block name")

    ASTNode NodeBlockName::getASTNode(TokenReader &tokenReader, const CPack &cpack) const {
        return getStringASTNode(tokenReader);
    }

} // CHelper::Node