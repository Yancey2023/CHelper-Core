//
// Created by Yancey on 2023/12/22.
//

#ifndef CHELPER_NODEENTRY_H
#define CHELPER_NODEENTRY_H


#include "../NodeBase.h"

namespace CHelper::Node {

    class NodeEntry : public NodeBase {
    public:
        const std::shared_ptr<NodeBase> nodeKey;
        const std::shared_ptr<NodeBase> nodeSeparator;
        const std::shared_ptr<NodeBase> nodeValue;

        NodeEntry(const std::optional<std::string> &id,
                  const std::optional<std::string> &description,
                  const std::shared_ptr<NodeBase> &nodeKey,
                  const std::shared_ptr<NodeBase> &nodeSeparator,
                  const std::shared_ptr<NodeBase> &nodeValue);

        ASTNode getASTNode(TokenReader &tokenReader) const override;

        std::optional<std::string> collectDescription(const ASTNode *node, size_t index) const override;

    };

} // CHelper::Node


#endif //CHELPER_NODEENTRY_H
