//
// Created by Yancey666 on 2023/12/2.
//

#ifndef CHELPER_NODETEXT_H
#define CHELPER_NODETEXT_H

#include "../NodeBase.h"
#include "../../resources/id/NamespaceId.h"

namespace CHelper::Node {

    class NodeText : public NodeBase {
        NormalId data;

    public:
        NodeText(const std::optional<std::string> &id,
                 const std::optional<std::string> &description,
                 const NormalId& data);

        explicit NodeText(const nlohmann::json &j,
                          const CPack &cpack);

        NODE_TYPE_H;

        void toJson(nlohmann::json &j) const override;

        ASTNode getASTNode(TokenReader &tokenReader, const CPack &cpack) const override;

        bool collectIdError(const ASTNode *astNode,
                            const CPack &cpack,
                            std::vector<std::shared_ptr<ErrorReason>> &idErrorReasons) const override;
    };
} // CHelper::Node

#endif //CHELPER_NODETEXT_H
