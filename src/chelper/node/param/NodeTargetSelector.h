//
// Created by Yancey on 2023/12/1.
//

#pragma once

#ifndef CHELPER_NODETARGETSELECTOR_H
#define CHELPER_NODETARGETSELECTOR_H

#include "pch.h"
#include "../NodeBase.h"
#include "../util/NodeAnd.h"
#include "../util/NodeOr.h"
#include "../selector/NodeTargetSelectorArgument.h"
#include "../util/NodeList.h"

namespace CHelper::Node {

    class NodeTargetSelector : public NodeBase {
    public:
        const bool isMustPlayer, isMustNPC, isOnlyOne;
        const NodeNamespaceId nodeItem;
        const NodeNormalId nodeFamily, nodeGameMode, nodeItemLocation;
        const NodeNamespaceId nodeEntities;
        const NodeTargetSelectorArgument nodeArgument;
        const NodeList nodeArguments;

        NodeTargetSelector(const std::optional<std::string> &id,
                           const std::optional<std::string> &description,
                           bool isMustPlayer,
                           bool isMustNPC,
                           bool isOnlyOne,
                           const NodeNamespaceId &nodeItem,
                           const NodeNormalId &nodeFamily,
                           const NodeNormalId &nodeGameMode,
                           const NodeNormalId &nodeItemLocation,
                           const NodeNamespaceId &nodeEntities);

        NodeTargetSelector(const nlohmann::json &j,
                           [[maybe_unused]] const CPack &cpack);

        [[nodiscard]] NodeType *getNodeType() const override;

        void toJson(nlohmann::json &j) const override;

        ASTNode getASTNode(TokenReader &tokenReader, const CPack *cpack) const override;

        bool collectSuggestions(const ASTNode *astNode,
                                size_t index,
                                std::vector<Suggestions> &suggestions) const override;

        void collectStructure(const ASTNode *astNode,
                              StructureBuilder &structure,
                              bool isMustHave) const override;

    };

} // CHelper::Node

#endif //CHELPER_NODETARGETSELECTOR_H
