//
// Created by Yancey on 2023/12/1.
//

#pragma once

#ifndef CHELPER_NODETARGETSELECTOR_H
#define CHELPER_NODETARGETSELECTOR_H

#include "../NodeBase.h"
#include "../selector/NodeTargetSelectorArgument.h"
#include "../util/NodeAnd.h"
#include "../util/NodeList.h"
#include "../util/NodeOr.h"
#include "pch.h"

namespace CHelper::Node {

    class NodeTargetSelector : public NodeBase {
    public:
        bool isMustPlayer, isMustNPC, isOnlyOne;

    private:
        std::unique_ptr<NodeNamespaceId> nodeItem;
        std::unique_ptr<NodeNormalId> nodeFamily, nodeGameMode, nodeItemLocation;
        std::unique_ptr<NodeNamespaceId> nodeEntities;
        std::unique_ptr<NodeTargetSelectorArgument> nodeArgument;
        std::unique_ptr<NodeList> nodeArguments;

    public:
        NodeTargetSelector() = default;

        [[nodiscard]] NodeType *getNodeType() const override;

        ASTNode getASTNode(TokenReader &tokenReader, const CPack *cpack) const override;

        bool collectSuggestions(const ASTNode *astNode,
                                size_t index,
                                std::vector<Suggestions> &suggestions) const override;

        void collectStructure(const ASTNode *astNode,
                              StructureBuilder &structure,
                              bool isMustHave) const override;
        void init(const CPack &cpack) override;
    };

    CODEC_NODE_H(NodeTargetSelector)

}// namespace CHelper::Node

#endif//CHELPER_NODETARGETSELECTOR_H
