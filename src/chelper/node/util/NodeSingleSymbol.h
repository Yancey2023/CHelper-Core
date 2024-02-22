//
// Created by Yancey666 on 2023/12/19.
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

        NodeSingleSymbol(const nlohmann::json &j,
                         const CPack &cpack);

        NODE_TYPE_H

        ASTNode getASTNode(TokenReader &tokenReader, const CPack &cpack) const override;

    };

} // CHelper::Node


#endif //CHELPER_NODESINGLESYMBOL_H
