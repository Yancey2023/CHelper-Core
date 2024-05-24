//
// Created by Yancey on 2023/11/10.
//

#pragma once

#ifndef CHELPER_NODEPERCOMMAND_H
#define CHELPER_NODEPERCOMMAND_H

#include "../NodeBase.h"
#include "../util/NodeOr.h"

namespace CHelper::Node {

    class NodePerCommand : public NodeBase {
    public:
        std::vector<std::string> name;
        std::vector<std::unique_ptr<Node::NodeBase>> nodes;
        std::vector<Node::NodeBase *> startNodes;

        NodePerCommand() = default;

        void init(const CPack &cpack) override;

        [[nodiscard]] NodeType *getNodeType() const override;

        ASTNode getASTNode(TokenReader &tokenReader, const CPack *cpack) const override;

        std::optional<std::string> collectDescription(const ASTNode *node, size_t index) const override;

    };// NodePerCommand::Node

    CODEC_H(std::unique_ptr<NodePerCommand>)

}// namespace CHelper::Node

#endif//CHELPER_NODEPERCOMMAND_H
