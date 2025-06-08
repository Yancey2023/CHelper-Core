//
// Created by Yancey on 2024/2/27.
//

#pragma once

#ifndef CHELPER_NODERANGE_H
#define CHELPER_NODERANGE_H

#include "../NodeBase.h"

namespace CHelper::Node {

    class NodeRange : public NodeBase {
    public:
        NodeRange() = default;

        NodeRange(const std::optional<std::u16string> &id,
                  const std::optional<std::u16string> &description);

        [[nodiscard]] NodeTypeId::NodeTypeId getNodeType() const override;

        ASTNode getASTNode(TokenReader &tokenReader, const CPack *cpack) const override;

        bool collectSuggestions(const ASTNode *astNode,
                                size_t index,
                                std::vector<Suggestions> &suggestions) const override;

        void collectStructure(const ASTNode *astNode,
                              StructureBuilder &structure,
                              bool isMustHave) const override;

        bool collectSyntax(const ASTNode *astNode,
                           SyntaxResult &syntaxResult) const override;
    };

}// namespace CHelper::Node

CODEC_NODE_NONE(CHelper::Node::NodeRange)

#endif//CHELPER_NODERANGE_H
