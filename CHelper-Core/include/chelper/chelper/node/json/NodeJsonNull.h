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
                     const std::optional<std::u16string> &description);

        [[nodiscard]] NodeTypeId::NodeTypeId getNodeType() const override;

        ASTNode getASTNode(TokenReader &tokenReader, const CPack *cpack, void *private_data = nullptr) const override;

        bool collectSuggestions(const ASTNode *astNode,
                                size_t index,
                                std::vector<Suggestions> &suggestions) const override;

        bool collectSyntax(const ASTNode *astNode,
                           SyntaxResult &syntaxResult) const override;
    };

}// namespace CHelper::Node

CODEC_NODE_NONE(CHelper::Node::NodeJsonNull)

#endif//CHELPER_NODEJSONNULL_H
