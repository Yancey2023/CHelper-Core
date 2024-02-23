//
// Created by Yancey on 2023/12/18.
//

#ifndef CHELPER_NODEBLOCKSTATE_H
#define CHELPER_NODEBLOCKSTATE_H

#include "../NodeBase.h"

namespace CHelper::Node {

    class NodeBlockState : public NodeBase {
    public:
        NODE_TYPE_INSTANCE_H(NodeBlockState);

        ASTNode getASTNode(TokenReader &tokenReader, const CPack &cpack) const override;

        void collectStructure(const ASTNode *astNode,
                              StructureBuilder &structure,
                              bool isMustHave) const override;

    };

} // CHelper::Node

#endif //CHELPER_NODEBLOCKSTATE_H
