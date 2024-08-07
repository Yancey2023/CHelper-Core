//
// Created by Yancey on 2023/11/11.
//

#pragma once

#ifndef CHELPER_NODEBOOLEAN_H
#define CHELPER_NODEBOOLEAN_H

#include "../NodeBase.h"

namespace CHelper::Node {

    class NodeBoolean : public NodeBase {
    public:
        std::optional<std::string> descriptionTrue, descriptionFalse;

        NodeBoolean() = default;

        NodeBoolean(const std::optional<std::string> &id,
                    const std::optional<std::string> &description,
                    const std::optional<std::string> &descriptionTrue,
                    const std::optional<std::string> &descriptionFalse);

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

    CODEC_NODE_H(NodeBoolean)

}// namespace CHelper::Node

#endif//CHELPER_NODEBOOLEAN_H
