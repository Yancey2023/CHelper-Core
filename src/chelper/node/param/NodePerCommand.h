//
// Created by Yancey666 on 2023/11/10.
//

#ifndef CHELPER_NODEPERCOMMAND_H
#define CHELPER_NODEPERCOMMAND_H

#include "pch.h"
#include "../NodeBase.h"
#include "../util/NodeOr.h"

namespace CHelper::Node {

    class NodePerCommand : public NodeBase {
    public:
        std::vector<std::string> name;
        std::vector<std::shared_ptr<Node::NodeBase>> nodes;
        std::vector<std::shared_ptr<Node::NodeBase>> startNodes;

    public:
        NodePerCommand(const std::optional<std::string> &id,
                       std::vector<std::string> name,
                       const std::optional<std::string> &description,
                       std::vector<std::shared_ptr<Node::NodeBase>> nodes,
                       std::vector<std::shared_ptr<Node::NodeBase>> startNodes);

        NodePerCommand(const nlohmann::json &j,
                       const CPack &cpack);

        NODE_TYPE_H;

        void toJson(nlohmann::json &j) const override;

        ASTNode getASTNode(TokenReader &tokenReader, const CPack &cpack) const override;

        std::optional<std::string> getDescription(const ASTNode *pNode, size_t index) const override;

    }; // NodePerCommand::Node

} // CHelper

CREATE_ADL_SERIALIZER(CHelper::Node::NodePerCommand);

#endif //CHELPER_NODEPERCOMMAND_H
