//
// Created by Yancey on 2023/11/11.
//

#pragma once

#ifndef CHELPER_NODEFLOAT_H
#define CHELPER_NODEFLOAT_H

#include "../NodeBase.h"

namespace CHelper::Node {

    class NodeFloat : public NodeBase {
    public:
        std::optional<float> min, max;

        NodeFloat() = default;

        NodeFloat(const std::optional<std::string> &id,
                  const std::optional<std::string> &description,
                  const std::optional<float> &min,
                  const std::optional<float> &max);

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

    CODEC_NODE_H(NodeFloat)

}// namespace CHelper::Node

#endif//CHELPER_NODEFLOAT_H
