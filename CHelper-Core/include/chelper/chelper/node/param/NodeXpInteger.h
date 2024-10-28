//
// Created by Yancey on 2024/3/1.
//

#pragma once

#ifndef CHELPER_NODEXPINTEGER_H
#define CHELPER_NODEXPINTEGER_H

#include "../NodeBase.h"

namespace CHelper::Node {

    class NodeXpInteger : public NodeBase {
    public:
        NodeXpInteger() = default;

        [[nodiscard]] NodeTypeId::NodeTypeId getNodeType() const override;

        ASTNode getASTNode(TokenReader &tokenReader, const CPack *cpack) const override;

        void collectStructure(const ASTNode *astNode,
                              StructureBuilder &structure,
                              bool isMustHave) const override;

        bool collectColor(const ASTNode *astNode,
                          ColoredString &coloredString,
                          const Theme &theme) const override;
    };

}// namespace CHelper::Node

CODEC_NODE_NONE(CHelper::Node::NodeXpInteger)

#endif//CHELPER_NODEXPINTEGER_H
