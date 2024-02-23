//
// Created by Yancey on 2024/2/20.
//

#include "NodeTargetSelectorArgument.h"

namespace CHelper::Node {

    NODE_TYPE_INSTANCE("TARGET_SELECTOR_ARGUMENT", NodeTargetSelectorArgument, "目标选择器参数")

    ASTNode NodeTargetSelectorArgument::getASTNode(TokenReader &tokenReader, const CPack &cpack) const {
        //TODO 目标选择器参数
        return getStringASTNode(tokenReader);
    }

} // CHelper::Node