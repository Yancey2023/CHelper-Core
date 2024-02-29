//
// Created by Yancey on 2023/12/18.
//

#ifndef CHELPER_NODEBLOCKSTATE_H
#define CHELPER_NODEBLOCKSTATE_H

#include "../NodeBase.h"

namespace CHelper::Node {

    class NodeBlockState : public NodeBase {
    public:
        NodeBlockState(const std::optional<std::string> &id, const std::optional<std::string> &description);

        static std::shared_ptr<NodeBlockState> getInstance();

        ASTNode getASTNode(TokenReader &tokenReader, const CPack &cpack) const override;

        void collectStructure(const ASTNode *astNode,
                              StructureBuilder &structure,
                              bool isMustHave) const override;

    };

} // CHelper::Node

#endif //CHELPER_NODEBLOCKSTATE_H
