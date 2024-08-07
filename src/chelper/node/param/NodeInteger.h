//
// Created by Yancey on 2023/11/11.
//

#pragma once

#ifndef CHELPER_NODEINTEGER_H
#define CHELPER_NODEINTEGER_H

#include "../NodeBase.h"

namespace CHelper::Node {

    class NodeInteger : public NodeBase {
    public:
        std::optional<int32_t> min, max;

        NodeInteger() = default;

        NodeInteger(const std::optional<std::string> &id,
                    const std::optional<std::string> &description,
                    const std::optional<int32_t> &min,
                    const std::optional<int32_t> &max);

        [[nodiscard]] NodeType *getNodeType() const override;

        ASTNode getASTNode(TokenReader &tokenReader, const CPack *cpack) const override;

        bool collectIdError(const ASTNode *astNode,
                            std::vector<std::shared_ptr<ErrorReason>> &idErrorReasons) const override;

        void collectStructure(const ASTNode *astNode,
                              StructureBuilder &structure,
                              bool isMustHave) const override;

        bool collectColor(const ASTNode *astNode,
                          ColoredString &coloredString,
                          const Theme &theme) const override;
    };

    CODEC_H(NodeInteger)

}// namespace CHelper::Node

#endif//CHELPER_NODEINTEGER_H
