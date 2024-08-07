//
// Created by Yancey on 2024/2/28.
//

#pragma once

#ifndef CHELPER_NODEJSONNULL_H
#define CHELPER_NODEJSONNULL_H

#include "../NodeBase.h"

namespace CHelper::Node {

    class NodeJsonNull : public NodeBase {
    public:
        NodeJsonNull() = default;

        NodeJsonNull(const std::optional<std::string> &id,
                     const std::optional<std::string> &description);

        [[nodiscard]] NodeType *getNodeType() const override;

        ASTNode getASTNode(TokenReader &tokenReader, const CPack *cpack) const override;

        bool collectSuggestions(const ASTNode *astNode,
                                size_t index,
                                std::vector<Suggestions> &suggestions) const override;

        bool collectColor(const ASTNode *astNode,
                          ColoredString &coloredString,
                          const Theme &theme) const override;
    };

    CODEC_UNIQUE_PTR_H(NodeJsonNull)

}// namespace CHelper::Node

#endif//CHELPER_NODEJSONNULL_H
