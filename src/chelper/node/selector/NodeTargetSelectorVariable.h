//
// Created by Yancey on 2024/2/14.
//

#ifndef CHELPER_NODETARGETSELECTORVARIABLE_H
#define CHELPER_NODETARGETSELECTORVARIABLE_H

#include "pch.h"
#include "../NodeBase.h"

namespace CHelper::Node {

    class NodeTargetSelectorVariable : public NodeBase {
    public:
        const bool isMustPlayer, isMustNPC, isOnlyOne;

        NodeTargetSelectorVariable(const std::optional<std::string> &id,
                                   const std::optional<std::string> &description,
                                   bool isMustPlayer,
                                   bool isMustNPC,
                                   bool isOnlyOne);

        ASTNode getASTNode(TokenReader &tokenReader) const override;

        bool collectIdError(const ASTNode *astNode,
                            std::vector<std::shared_ptr<ErrorReason>> &idErrorReasons) const override;
    };

} // CHelper::Node

#endif //CHELPER_NODETARGETSELECTORVARIABLE_H
