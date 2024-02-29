//
// Created by Yancey on 2024/2/28.
//

#ifndef CHELPER_NODEJSONLIST_H
#define CHELPER_NODEJSONLIST_H

#include "../NodeBase.h"

namespace CHelper::Node {

    class NodeJsonList : public NodeBase {
    public:
        const std::string data;
        std::shared_ptr<NodeBase> nodeList;

        NodeJsonList(const std::optional<std::string> &id,
                     const std::optional<std::string> &description,
                     std::string data);

        NodeJsonList(const nlohmann::json &j,
                     [[maybe_unused]] const CPack &cpack);

        void init(const std::vector<std::shared_ptr<NodeBase>> &dataList);

        [[nodiscard]] std::shared_ptr<NodeType> getNodeType() const override;

        ASTNode getASTNode(TokenReader &tokenReader, const CPack &cpack) const override;

    };

} // CHelper::Node

#endif //CHELPER_NODEJSONLIST_H
