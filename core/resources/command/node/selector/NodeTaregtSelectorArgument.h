//
// Created by Yancey666 on 2024/2/20.
//

#ifndef CHELPER_NODETAREGTSELECTORARGUMENT_H
#define CHELPER_NODETAREGTSELECTORARGUMENT_H

#include "../NodeBase.h"

namespace CHelper::Node {

    class NodeTargetSelectorArgument : public NodeBase {
    public:

        NODE_TYPE_INSTANCE_H(NodeTargetSelectorArgument);

        ASTNode getASTNode(TokenReader &tokenReader, const CPack &cpack) const override;
    };

} // CHelper::Node

#endif //CHELPER_NODETAREGTSELECTORARGUMENT_H