//
// Created by Yancey on 2023/12/22.
//

#pragma once

#ifndef CHELPER_NODEENTRY_H
#define CHELPER_NODEENTRY_H


#include "../NodeBase.h"

namespace CHelper::Node {

    class NodeEntry : public NodeBase {
    public:
        NodeBase *nodeKey = nullptr;
        NodeBase *nodeSeparator = nullptr;
        NodeBase *nodeValue = nullptr;

        NodeEntry() = default;

        NodeEntry(const std::optional<std::string> &id,
                  const std::optional<std::string> &description,
                  NodeBase *nodeKey,
                  NodeBase *nodeSeparator,
                  NodeBase *nodeValue);

        [[nodiscard]] NodeType *getNodeType() const override;

        ASTNode getASTNode(TokenReader &tokenReader, const CPack *cpack) const override;

        std::optional<std::string> collectDescription(const ASTNode *node, size_t index) const override;
    };

}// namespace CHelper::Node


#endif//CHELPER_NODEENTRY_H
