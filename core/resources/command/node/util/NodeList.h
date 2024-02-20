//
// Created by Yancey666 on 2023/12/23.
//

#ifndef CHELPER_NODELIST_H
#define CHELPER_NODELIST_H

#include "../NodeBase.h"

namespace CHelper::Node {

    class NodeList : public NodeBase {
    public:
        std::shared_ptr<NodeBase> nodeLeft;
        std::shared_ptr<NodeBase> nodeElement;
        std::shared_ptr<NodeBase> nodeSeparator;
        std::shared_ptr<NodeBase> nodeRight;

        NodeList(const std::optional<std::string> &id,
                 const std::optional<std::string> &description,
                 std::shared_ptr<NodeBase> &nodeLeft,
                 std::shared_ptr<NodeBase> &nodeElement,
                 std::shared_ptr<NodeBase> &nodeSeparator,
                 std::shared_ptr<NodeBase> &nodeRight);

        NodeList(const nlohmann::json &j,
                 const CPack &cpack);

        NODE_TYPE_H;

        ASTNode getASTNode(TokenReader &tokenReader, const CPack &cpack) const override;

    };

} // CHelper::Node
#endif //CHELPER_NODELIST_H
