//
// Created by Yancey666 on 2023/12/18.
//

#ifndef CHELPER_NODEBLOCKNAME_H
#define CHELPER_NODEBLOCKNAME_H

#include "../NodeBase.h"

namespace CHelper::Node {

    class NodeBlockName : public NodeBase {
    public:
        NODE_TYPE_INSTANCE_H(NodeBlockName);

        ASTNode getASTNode(TokenReader &tokenReader, const CPack &cpack) const override;

    };

} // CHelper::Node

#endif //CHELPER_NODEBLOCKNAME_H
