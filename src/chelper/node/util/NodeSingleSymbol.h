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
        wchar_t symbol = ' ';
        std::shared_ptr<NormalId> normalId;
        bool isAddWhitespace = false;

        NodeSingleSymbol() = default;

        NodeSingleSymbol(const std::optional<std::wstring> &id,
                         const std::optional<std::wstring> &description,
                         wchar_t symbol,
                         bool isAddWhitespace = true);

        [[nodiscard]] NodeType *getNodeType() const override;

        ASTNode getASTNode(TokenReader &tokenReader, const CPack *cpack) const override;

        bool collectSuggestions(const ASTNode *astNode,
                                size_t index,
                                std::vector<Suggestions> &suggestions) const override;

        bool collectColor(const ASTNode *astNode,
                          ColoredString &coloredString,
                          const Theme &theme) const override;
    };

}// namespace CHelper::Node


#endif//CHELPER_NODESINGLESYMBOL_H
