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
        const NodeBase *nodeLeft;
        const NodeBase *nodeElement;
        const NodeBase *nodeSeparator;
        const NodeBase *nodeRight;
        const NodeOr nodeElementOrRight;
        const NodeOr nodeSeparatorOrRight;

        NodeList(const std::optional<std::string> &id,
                 const std::optional<std::string> &description,
                 const NodeBase *nodeLeft,
                 const NodeBase *nodeElement,
                 const NodeBase *nodeSeparator,
                 const NodeBase *nodeRight);

        ASTNode getASTNode(TokenReader &tokenReader, const CPack *cpack) const override;

        std::optional<std::string> collectDescription(const ASTNode *node, size_t index) const override;

    };

} // CHelper::Node
#endif //CHELPER_NODELIST_H
