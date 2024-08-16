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

        NodeEntry(const std::optional<std::wstring> &id,
                  const std::optional<std::wstring> &description,
                  NodeBase *nodeKey,
                  NodeBase *nodeSeparator,
                  NodeBase *nodeValue);

        [[nodiscard]] NodeType *getNodeType() const override;

        ASTNode getASTNode(TokenReader &tokenReader, const CPack *cpack) const override;

        std::optional<std::wstring> collectDescription(const ASTNode *node, size_t index) const override;
    };

}// namespace CHelper::Node


#endif//CHELPER_NODEENTRY_H
