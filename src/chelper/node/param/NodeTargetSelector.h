//
// Created by Yancey on 2023/12/1.
//

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
        std::shared_ptr<NodeBase> nodeArgument, nodeArguments;

        NodeTargetSelector(const std::optional<std::string> &id,
                           const std::optional<std::string> &description,
                           bool isMustPlayer,
                           bool isMustNPC,
                           bool isOnlyOne,
                           const std::shared_ptr<NodeBase> &nodeItem,
                           const std::shared_ptr<NodeBase> &nodeFamily,
                           const std::shared_ptr<NodeBase> &nodeGameMode,
                           const std::shared_ptr<NodeBase> &nodeItemLocation,
                           const std::shared_ptr<NodeBase> &nodeEntities);

        explicit NodeTargetSelector(const nlohmann::json &j,
                                    [[maybe_unused]] const CPack &cpack);

        [[nodiscard]] NodeType getNodeType() const override;

        void toJson(nlohmann::json &j) const override;

        ASTNode getASTNode(TokenReader &tokenReader) const override;

        bool collectSuggestions(const ASTNode *astNode,
                                size_t index,
                                std::vector<Suggestion> &suggestions) const override;

        void collectStructure(const ASTNode *astNode,
                              StructureBuilder &structure,
                              bool isMustHave) const override;

    };

} // CHelper::Node

#endif //CHELPER_NODETARGETSELECTOR_H
