//
// Created by Yancey on 2023/12/23.
//

#ifndef CHELPER_NODELIST_H
#define CHELPER_NODELIST_H

#include "../NodeBase.h"

namespace CHelper::Node {

    class NodeList : public NodeBase {
    public:
        const std::shared_ptr<NodeBase> nodeLeft;
        const std::shared_ptr<NodeBase> nodeElement;
        const std::shared_ptr<NodeBase> nodeSeparator;
        const std::shared_ptr<NodeBase> nodeRight;
        const std::shared_ptr<NodeBase> nodeElementOrRight;
        const std::shared_ptr<NodeBase> nodeSeparatorOrRight;

        NodeList(const std::optional<std::string> &id,
                 const std::optional<std::string> &description,
                 const std::shared_ptr<NodeBase> &nodeLeft,
                 const std::shared_ptr<NodeBase> &nodeElement,
                 const std::shared_ptr<NodeBase> &nodeSeparator,
                 const std::shared_ptr<NodeBase> &nodeRight);

        ASTNode getASTNode(TokenReader &tokenReader) const override;

        std::optional<std::string> collectDescription(const ASTNode *node, size_t index) const override;

    };

} // CHelper::Node
#endif //CHELPER_NODELIST_H
