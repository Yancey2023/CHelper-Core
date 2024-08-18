//
// Created by Yancey on 2024/2/28.
//

#pragma once

#ifndef CHELPER_NODEJSONINTEGER_H
#define CHELPER_NODEJSONINTEGER_H

#include "../NodeBase.h"

namespace CHelper::Node {

    class NodeJsonInteger : public NodeBase {
    public:
        std::optional<int32_t> min, max;

        NodeJsonInteger() = default;

        NodeJsonInteger(const std::optional<std::wstring> &id,
                        const std::optional<std::wstring> &description,
                        const std::optional<int32_t> &min,
                        const std::optional<int32_t> &max);

        [[nodiscard]] NodeType *getNodeType() const override;

        ASTNode getASTNode(TokenReader &tokenReader, const CPack *cpack) const override;

        bool collectIdError(const ASTNode *astNode,
                            std::vector<std::shared_ptr<ErrorReason>> &idErrorReasons) const override;

        bool collectColor(const ASTNode *astNode,
                          ColoredString &coloredString,
                          const Theme &theme) const override;
    };

    CODEC_NODE_H(NodeJsonInteger)

}// namespace CHelper::Node

#endif//CHELPER_NODEJSONINTEGER_H
