//
// Created by Yancey on 2024/2/28.
//

#pragma once

#ifndef CHELPER_NODEJSONBOOLEAN_H
#define CHELPER_NODEJSONBOOLEAN_H

#include "../NodeBase.h"

namespace CHelper::Node {

    class NodeJsonBoolean : public NodeBase {
    public:
        std::optional<std::string> descriptionTrue, descriptionFalse;

        NodeJsonBoolean() = default;

        NodeJsonBoolean(const std::optional<std::string> &id,
                        const std::optional<std::string> &description,
                        const std::optional<std::string> &descriptionTrue,
                        const std::optional<std::string> &descriptionFalse);

        [[nodiscard]] NodeType *getNodeType() const override;

        ASTNode getASTNode(TokenReader &tokenReader, const CPack *cpack) const override;

        bool collectSuggestions(const ASTNode *astNode,
                                size_t index,
                                std::vector<Suggestions> &suggestions) const override;

        bool collectColor(const ASTNode *astNode,
                          ColoredString &coloredString,
                          const Theme &theme) const override;
    };

    CODEC_NODE_H(NodeJsonBoolean)

}// namespace CHelper::Node

#endif//CHELPER_NODEJSONBOOLEAN_H
