//
// Created by Yancey on 2023/12/1.
//

#pragma once

#ifndef CHELPER_NODETARGETSELECTOR_H
#define CHELPER_NODETARGETSELECTOR_H

#include "../NodeBase.h"
#include "../util/NodeAnd.h"
#include "../util/NodeEqualEntry.h"
#include "../util/NodeList.h"
#include "../util/NodeOr.h"
#include "NodeNamespaceId.h"
#include "NodeNormalId.h"
#include "pch.h"

namespace CHelper::Node {

    class NodeTargetSelector : public NodeBase {
    public:
        bool isMustPlayer = false, isMustNPC = false, isOnlyOne = false;

    private:
        std::unique_ptr<NodeNamespaceId> nodeItem;
        std::unique_ptr<NodeNormalId> nodeFamily, nodeGameMode, nodeSlot;
        std::unique_ptr<NodeNamespaceId> nodeEntities;
        std::unique_ptr<NodeEqualEntry> nodeHasItemElement;
        std::unique_ptr<NodeList> nodeHasItemList1, nodeHasItemList2;
        std::unique_ptr<NodeOr> nodeHasItem;
        std::unique_ptr<NodeEqualEntry> nodeArgument;
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
