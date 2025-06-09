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
                  const std::optional<std::u16string> &description,
                  NodeBase *nodeKey,
                  NodeBase *nodeSeparator,
                  NodeBase *nodeValue);

        [[nodiscard]] NodeTypeId::NodeTypeId getNodeType() const override;

        ASTNode getASTNode(TokenReader &tokenReader, const CPack *cpack = nullptr) const override;

        std::optional<std::u16string> collectDescription(const ASTNode *node, size_t index) const override;
    };

}// namespace CHelper::Node


#endif//CHELPER_NODEENTRY_H
