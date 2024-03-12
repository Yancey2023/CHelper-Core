//
// Created by Yancey on 2024/2/28.
//

#ifndef CHELPER_NODEJSONOBJECT_H
#define CHELPER_NODEJSONOBJECT_H

#include "../NodeBase.h"
#include "NodeJsonEntry.h"

namespace CHelper::Node {

    class NodeJsonObject : public NodeBase {
    public:
        const std::vector<std::unique_ptr<NodeBase>> data;
        std::unique_ptr<NodeBase> nodeElement;
        std::unique_ptr<NodeBase> nodeList;

        NodeJsonObject(const std::optional<std::string> &id,
                       const std::optional<std::string> &description,
                       std::vector<std::unique_ptr<NodeBase>> data);

        NodeJsonObject(const nlohmann::json &j,
                       [[maybe_unused]] const CPack &cpack);

        void init(const std::vector<std::unique_ptr<NodeBase>> &dataList);

        [[nodiscard]] NodeType *getNodeType() const override;

        ASTNode getASTNode(TokenReader &tokenReader, const CPack *cpack) const override;

        bool collectSuggestions(const ASTNode *astNode,
                                size_t index,
                                std::vector<Suggestion> &suggestions) const override;

    };

} // CHelper::Node

#endif //CHELPER_NODEJSONOBJECT_H
