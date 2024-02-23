//
// Created by Yancey on 2023/12/10.
//

#ifndef CHELPER_NODESTRING_H
#define CHELPER_NODESTRING_H

#include "pch.h"
#include "../NodeBase.h"

namespace CHelper::Node {

    class NodeString : public NodeBase {
    public:
        const bool canContainSpace;
        const bool ignoreLater;

        NodeString(const std::optional<std::string> &id,
                   const std::optional<std::string> &description,
                   bool canContainSpace,
                   bool ignoreLater);

        NodeString(const nlohmann::json &j,
                   const CPack &cpack);

        NODE_TYPE_H;

        ASTNode getASTNode(TokenReader &tokenReader, const CPack &cpack) const override;

        void collectStructure(const ASTNode *astNode,
                              StructureBuilder &structure,
                              bool isMustHave) const override;

    };

} // CHelper::Node

#endif //CHELPER_NODESTRING_H
