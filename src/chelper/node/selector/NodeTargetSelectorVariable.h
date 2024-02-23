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

        explicit NodeTargetSelectorVariable(const nlohmann::json &j,
                                            const CPack &cpack);

        NODE_TYPE_H;

        void toJson(nlohmann::json &j) const override;

        ASTNode getASTNode(TokenReader &tokenReader, const CPack &cpack) const override;

        bool collectIdError(const ASTNode *astNode,
                            const CPack &cpack,
                            std::vector<std::shared_ptr<ErrorReason>> &idErrorReasons) const override;
    };

} // CHelper::Node

#endif //CHELPER_NODETARGETSELECTORVARIABLE_H
