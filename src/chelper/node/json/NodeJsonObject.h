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
        const std::shared_ptr<std::vector<std::shared_ptr<NodeBase>>> data;
        std::shared_ptr<NodeBase> nodeList;

        NodeJsonObject(const std::optional<std::string> &id,
                       const std::optional<std::string> &description,
                       const std::shared_ptr<std::vector<std::shared_ptr<NodeBase>>> &data);

        NodeJsonObject(const nlohmann::json &j,
                       [[maybe_unused]] const CPack &cpack);

        void init(const std::vector<std::shared_ptr<NodeBase>> &dataList);

        [[nodiscard]] std::shared_ptr<NodeType> getNodeType() const override;

        ASTNode getASTNode(TokenReader &tokenReader) const override;

    };

} // CHelper::Node

#endif //CHELPER_NODEJSONOBJECT_H
