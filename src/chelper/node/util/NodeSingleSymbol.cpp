//
// Created by Yancey on 2023/12/19.
//

#include "NodeSingleSymbol.h"

namespace CHelper::Node {

    NodeSingleSymbol::NodeSingleSymbol(const std::optional<std::string> &id,
                                       const std::optional<std::string> &description,
                                       char symbol)
            : NodeBase(id, description),
              symbol(symbol) {}

    ASTNode NodeSingleSymbol::getASTNode(TokenReader &tokenReader, const CPack &cpack) const {
        return getSymbolASTNode(tokenReader, symbol);
    }

} // CHelper::Node