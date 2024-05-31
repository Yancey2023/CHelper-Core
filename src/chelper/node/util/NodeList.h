//
// Created by Yancey on 2023/12/23.
//

#pragma once

#ifndef CHELPER_NODELIST_H
#define CHELPER_NODELIST_H

#include "../NodeBase.h"
#include "NodeOr.h"

namespace CHelper::Node {

    class NodeList : public NodeBase {
    public:
        NodeBase *nodeLeft = nullptr;
        NodeBase *nodeElement = nullptr;
        NodeBase *nodeSeparator = nullptr;
        NodeBase *nodeRight = nullptr;
        NodeOr nodeElementOrRight;
        NodeOr nodeSeparatorOrRight;

        NodeList() = default;

        NodeList(const std::optional<std::string> &id,
                 const std::optional<std::string> &description,
                 NodeBase *nodeLeft,
                 NodeBase *nodeElement,
                 NodeBase *nodeSeparator,
                 NodeBase *nodeRight);

        [[nodiscard]] NodeType *getNodeType() const override;

        ASTNode getASTNode(TokenReader &tokenReader, const CPack *cpack) const override;

        std::optional<std::string> collectDescription(const ASTNode *node, size_t index) const override;
    };

}// namespace CHelper::Node
#endif//CHELPER_NODELIST_H
