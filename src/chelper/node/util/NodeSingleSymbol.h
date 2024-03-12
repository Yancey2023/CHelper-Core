//
// Created by Yancey on 2023/12/19.
//

#ifndef CHELPER_NODESINGLESYMBOL_H
#define CHELPER_NODESINGLESYMBOL_H


#include "../NodeBase.h"

namespace CHelper::Node {

    class NodeSingleSymbol : public NodeBase {
    public:
        char symbol;

        NodeSingleSymbol(const std::optional<std::string> &id,
                         const std::optional<std::string> &description,
                         char symbol);

        ASTNode getASTNode(TokenReader &tokenReader, const CPack *cpack) const override;

        bool collectSuggestions(const ASTNode *astNode,
                                size_t index,
                                std::vector<Suggestion> &suggestions) const override;

    };

} // CHelper::Node


#endif //CHELPER_NODESINGLESYMBOL_H
