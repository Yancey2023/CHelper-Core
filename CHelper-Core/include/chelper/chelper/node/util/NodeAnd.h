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
            NORMAL,
            NO_WHITESPACE
        };
    }// namespace WhitespaceMode


    class NodeAnd : public NodeBase {
    public:
        WhitespaceMode::WhitespaceMode whitespaceMode = WhitespaceMode::NO_WHITESPACE;
        std::vector<const NodeBase *> childNodes;

        NodeAnd() = default;

        NodeAnd(const std::optional<std::u16string> &id,
                const std::optional<std::u16string> &description,
                WhitespaceMode::WhitespaceMode whitespaceMode,
                const std::vector<const NodeBase *> &childNodes);

        [[nodiscard]] NodeTypeId::NodeTypeId getNodeType() const override;

        ASTNode getASTNode(TokenReader &tokenReader, const CPack *cpack) const override;

        std::optional<std::u16string> collectDescription(const ASTNode *node, size_t index) const override;
    };

}// namespace CHelper::Node

#endif//CHELPER_NODEAND_H
