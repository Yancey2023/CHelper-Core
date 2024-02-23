//
// Created by Yancey on 2024/2/22.
//

#ifndef CHELPER_NODEITEMID_H
#define CHELPER_NODEITEMID_H

#include "../NodeBase.h"

namespace CHelper::Node {

    class NodeItemId : public NodeBase {
    public:
        NODE_TYPE_INSTANCE_H(NodeItemId);

        ASTNode getASTNode(TokenReader &tokenReader, const CPack &cpack) const override;

        bool collectIdError(const ASTNode *astNode,
                            const CPack &cpack,
                            std::vector<std::shared_ptr<ErrorReason>> &idErrorReasons) const override;

        bool collectSuggestions(const ASTNode *astNode,
                                const CPack &cpack,
                                std::vector<Suggestion> &suggestions) const override;

        void collectStructure(const ASTNode *astNode,
                              StructureBuilder &structure,
                              bool isMustHave) const override;

    };

} // CHelper::Node

#endif //CHELPER_NODEITEMID_H
