//
// Created by Yancey on 2023/11/10.
//

#pragma once

#ifndef CHELPER_NODEPERCOMMAND_H
#define CHELPER_NODEPERCOMMAND_H

#include "pch.h"
#include "../NodeBase.h"
#include "../util/NodeOr.h"

namespace CHelper::Node {

    class NodePerCommand : public NodeBase {
    public:
        std::vector<std::string> name;
        std::vector<std::unique_ptr<Node::NodeBase>> nodes;
        std::vector<Node::NodeBase *> startNodes;

    public:
        NodePerCommand(const std::optional<std::string> &id,
                       std::vector<std::string> name,
                       const std::optional<std::string> &description,
                       std::vector<std::unique_ptr<Node::NodeBase>> nodes,
                       std::vector<Node::NodeBase *> &startNodes);

        NodePerCommand(const nlohmann::json &j,
                       const CPack &cpack);

        [[nodiscard]] NodeType *getNodeType() const override;

        void toJson(nlohmann::json &j) const override;

        ASTNode getASTNode(TokenReader &tokenReader, const CPack *cpack) const override;

        std::optional<std::string> collectDescription(const ASTNode *node, size_t index) const override;

    }; // NodePerCommand::Node

} // CHelper

#endif //CHELPER_NODEPERCOMMAND_H
