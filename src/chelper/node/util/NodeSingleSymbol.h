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
        char symbol;
        std::shared_ptr<NormalId> normalId;

        NodeSingleSymbol(const std::optional<std::string> &id,
                         const std::optional<std::string> &description,
                         char symbol);

        ASTNode getASTNode(TokenReader &tokenReader, const CPack *cpack) const override;

        bool collectSuggestions(const ASTNode *astNode,
                                size_t index,
                                std::vector<Suggestions> &suggestions) const override;
    };

}// namespace CHelper::Node


#endif//CHELPER_NODESINGLESYMBOL_H
