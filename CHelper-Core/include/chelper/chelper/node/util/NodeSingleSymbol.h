//
// Created by Yancey on 2023/12/19.
//

#pragma once

#ifndef CHELPER_NODESINGLESYMBOL_H
#define CHELPER_NODESINGLESYMBOL_H

#include "../NodeBase.h"

namespace CHelper::Node {

    class NodeSingleSymbol : public NodeBase {
    public:
        char16_t symbol = ' ';
        std::shared_ptr<NormalId> normalId;
        bool isAddSpace = false;

        NodeSingleSymbol() = default;

        NodeSingleSymbol(char16_t symbol,
                         const std::optional<std::u16string> &description,
                         bool isAddSpace = true);

        [[nodiscard]] NodeTypeId::NodeTypeId getNodeType() const override;

        ASTNode getASTNode(TokenReader &tokenReader, const CPack *cpack = nullptr) const override;

        bool collectSuggestions(const ASTNode *astNode,
                                size_t index,
                                Suggestions &suggestions) const override;

        bool collectSyntax(const ASTNode *astNode,
                           SyntaxResult &syntaxResult) const override;
    };

}// namespace CHelper::Node


#endif//CHELPER_NODESINGLESYMBOL_H
