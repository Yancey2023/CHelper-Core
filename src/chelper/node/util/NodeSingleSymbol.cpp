//
// Created by Yancey on 2023/12/19.
//

#include "NodeSingleSymbol.h"

namespace CHelper::Node {

    NODE_TYPE("SINGLE_SYMBOL", NodeSingleSymbol)

    NodeSingleSymbol::NodeSingleSymbol(const std::optional<std::string> &id,
                                       const std::optional<std::string> &description,
                                       char symbol)
            : NodeBase(id, description),
              symbol(symbol) {}

    NodeSingleSymbol::NodeSingleSymbol(const nlohmann::json &j,
                                       const CPack &cpack) :
            NodeBase(j, cpack),
            symbol(FROM_JSON(j, symbol, char)) {}

    ASTNode NodeSingleSymbol::getASTNode(TokenReader &tokenReader, const CPack &cpack) const {
        return getSymbolASTNode(tokenReader, symbol);
    }

} // CHelper::Node