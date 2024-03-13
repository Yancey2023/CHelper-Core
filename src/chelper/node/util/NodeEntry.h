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
        const NodeBase *nodeKey;
        const NodeBase *nodeSeparator;
        const NodeBase *nodeValue;
        const bool isIgnoreSuggestion = false;

        NodeEntry(const std::optional<std::string> &id,
                  const std::optional<std::string> &description,
                  const NodeBase *nodeKey,
                  const NodeBase *nodeSeparator,
                  const NodeBase *nodeValue);

        ASTNode getASTNode(TokenReader &tokenReader, const CPack *cpack) const override;

        std::optional<std::string> collectDescription(const ASTNode *node, size_t index) const override;

    };

} // CHelper::Node


#endif //CHELPER_NODEENTRY_H
