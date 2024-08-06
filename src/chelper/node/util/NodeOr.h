//
// Created by Yancey on 2023/12/22.
//

#pragma once

#ifndef CHELPER_NODEOR_H
#define CHELPER_NODEOR_H

#include "../NodeBase.h"

namespace CHelper::Node {

    class NodeOr : public NodeBase {
    public:
        std::vector<const NodeBase *> childNodes;
        bool isAttachToEnd = false, isUseFirst = false;
        ASTNodeId::ASTNodeId nodeId = ASTNodeId::NONE;
        bool noSuggestion = false;
        const char *defaultErrorReason = nullptr;

        NodeOr() = default;

        NodeOr(const std::optional<std::string> &id,
               const std::optional<std::string> &description,
               std::vector<const NodeBase *> childNodes,
               bool isAttachToEnd,
               bool isUseFirst = false,
               bool noSuggestion = false,
               const char *defaultErrorReason = nullptr,
               ASTNodeId::ASTNodeId nodeId = ASTNodeId::NONE);

        [[nodiscard]] NodeType *getNodeType() const override;

        ASTNode getASTNode(TokenReader &tokenReader, const CPack *cpack) const override;

        std::optional<std::string> collectDescription(const ASTNode *node, size_t index) const override;

        bool collectSuggestions(const ASTNode *astNode,
                                size_t index,
                                std::vector<Suggestions> &suggestions) const override;
    };

}// namespace CHelper::Node


#endif//CHELPER_NODEOR_H
