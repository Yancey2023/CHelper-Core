//
// Created by Yancey on 2023/12/22.
//

#pragma once

#ifndef CHELPER_NODEAND_H
#define CHELPER_NODEAND_H

#include "../NodeBase.h"

namespace CHelper::Node {

    namespace WhitespaceMode {
        enum WhitespaceMode : uint8_t {
            NORMAL, NO_WHITESPACE
        };
    }// namespace WhitespaceMode


    class NodeAnd : public NodeBase {
    public:
        WhitespaceMode::WhitespaceMode whitespaceMode;
        std::vector<const NodeBase *> childNodes;

        NodeAnd() = default;

        NodeAnd(const std::optional<std::string> &id,
                const std::optional<std::string> &description,
                WhitespaceMode::WhitespaceMode whitespaceMode,
                const std::vector<const NodeBase *> &childNodes);

        [[nodiscard]] NodeType *getNodeType() const override;

        ASTNode getASTNode(TokenReader &tokenReader, const CPack *cpack) const override;

        std::optional<std::string> collectDescription(const ASTNode *node, size_t index) const override;
    };

}// namespace CHelper::Node

#endif//CHELPER_NODEAND_H
