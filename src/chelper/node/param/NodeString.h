//
// Created by Yancey on 2023/12/10.
//

#pragma once

#ifndef CHELPER_NODESTRING_H
#define CHELPER_NODESTRING_H

#include "../NodeBase.h"
#include "pch.h"

namespace CHelper::Node {

    class NodeString : public NodeBase {
    public:
        bool allowMissingString;
        bool canContainSpace;
        bool ignoreLater;

        NodeString() = default;

        NodeString(const std::optional<std::string> &id,
                   const std::optional<std::string> &description,
                   bool allowMissingString,
                   bool canContainSpace,
                   bool ignoreLater);

        [[nodiscard]] NodeType *getNodeType() const override;

        ASTNode getASTNode(TokenReader &tokenReader, const CPack *cpack) const override;

        bool collectSuggestions(const ASTNode *astNode,
                                size_t index,
                                std::vector<Suggestions> &suggestions) const override;

        void collectStructure(const ASTNode *astNode,
                              StructureBuilder &structure,
                              bool isMustHave) const override;
    };

    CODEC_NODE_H(NodeString)

}// namespace CHelper::Node

#endif//CHELPER_NODESTRING_H
