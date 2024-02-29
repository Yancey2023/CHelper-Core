//
// Created by Yancey on 2024/2/28.
//

#ifndef CHELPER_NODEJSONSTRING_H
#define CHELPER_NODEJSONSTRING_H

#include "../NodeBase.h"

namespace CHelper::Node {

    class NodeJsonString : public NodeBase {
    public:
        const std::shared_ptr<NodeBase> data;

        NodeJsonString(const std::optional<std::string> &id,
                       const std::optional<std::string> &description,
                       const std::shared_ptr<NodeBase> &data);

        NodeJsonString(const nlohmann::json &j,
                       const CPack &cpack);

        [[nodiscard]] std::shared_ptr<NodeType> getNodeType() const override;

        void toJson(nlohmann::json &j) const override;

        ASTNode getASTNode(TokenReader &tokenReader, const CPack &cpack) const override;

        bool collectIdError(const ASTNode *astNode,
                            std::vector<std::shared_ptr<ErrorReason>> &idErrorReasons) const override;

        bool collectSuggestions(const ASTNode *astNode,
                                size_t index,
                                std::vector<Suggestion> &suggestions) const override;

    };

} // CHelper::Node

#endif //CHELPER_NODEJSONSTRING_H