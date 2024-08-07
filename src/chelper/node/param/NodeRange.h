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

        NodeRange(const std::optional<std::string> &id,
                  const std::optional<std::string> &description);

        [[nodiscard]] NodeType *getNodeType() const override;

        ASTNode getASTNode(TokenReader &tokenReader, const CPack *cpack) const override;

        bool collectSuggestions(const ASTNode *astNode,
                                size_t index,
                                std::vector<Suggestions> &suggestions) const override;

        void collectStructure(const ASTNode *astNode,
                              StructureBuilder &structure,
                              bool isMustHave) const override;

        bool collectColor(const ASTNode *astNode,
                          ColoredString &coloredString,
                          const Theme &theme) const override;
    };

    CODEC_UNIQUE_PTR_H(NodeRange)

}// namespace CHelper::Node

#endif//CHELPER_NODERANGE_H
