//
// Created by Yancey on 2023/11/11.
//

#pragma once

#ifndef CHELPER_NODECOMMAND_H
#define CHELPER_NODECOMMAND_H

#include "../NodeBase.h"
#include "../util/NodeOr.h"
#include "NodePerCommand.h"

namespace CHelper::Node {

    class NodeCommand : public NodeBase {
    public:
        const std::vector<std::unique_ptr<Node::NodeBase>> *commands;

        NodeCommand(const std::optional<std::string> &id,
                    const std::optional<std::string> &description,
                    const std::vector<std::unique_ptr<Node::NodeBase>> *commands);

        NodeCommand(const std::optional<std::string> &id,
                    const std::optional<std::string> &description,
                    [[maybe_unused]] const CPack &cpack);

        NodeCommand(const nlohmann::json &j,
                    [[maybe_unused]] const CPack &cpack);

        [[nodiscard]] NodeType *getNodeType() const override;

        ASTNode getASTNode(TokenReader &tokenReader, const CPack *cpack) const override;

        std::optional<std::string> collectDescription(const ASTNode *node, size_t index) const override;

        bool collectSuggestions(const ASTNode *astNode,
                                size_t index,
                                std::vector<Suggestions> &suggestions) const override;

        void collectStructure(const ASTNode *astNode,
                              StructureBuilder &structure,
                              bool isMustHave) const override;

    };

} // CHelper::Node

#endif //CHELPER_NODECOMMAND_H
