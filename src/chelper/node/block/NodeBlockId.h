//
// Created by Yancey666 on 2023/12/18.
//

#ifndef CHELPER_NODEBLOCKID_H
#define CHELPER_NODEBLOCKID_H

#include "../NodeBase.h"

namespace CHelper::Node {

    class NodeBlockId : public NodeBase {
    public:
        NODE_TYPE_INSTANCE_H(NodeBlockId);

        ASTNode getASTNode(TokenReader &tokenReader, const CPack &cpack) const override;

        bool collectIdError(const ASTNode *astNode,
                            const CPack &cpack,
                            std::vector<std::shared_ptr<ErrorReason>> &idErrorReasons) const override;

        bool collectSuggestions(const ASTNode *astNode,
                                        const CPack &cpack,
                                        std::vector<Suggestion> &vector) const override;

    };

} // CHelper::Node

#endif //CHELPER_NODEBLOCKID_H
