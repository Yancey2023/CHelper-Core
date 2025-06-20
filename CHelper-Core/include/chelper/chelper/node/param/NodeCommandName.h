//
// Created by Yancey on 2023/11/11.
//

#pragma once

#ifndef CHELPER_NODECOMMANDNAME_H
#define CHELPER_NODECOMMANDNAME_H

#include "../NodeBase.h"
#include "NodePerCommand.h"

namespace CHelper::Node {

    class NodeCommandName : public NodeSerializable {
    public:
        std::vector<std::unique_ptr<NodePerCommand>> *commands = nullptr;

        void init(const CPack &cpack) override;

        [[nodiscard]] NodeTypeId::NodeTypeId getNodeType() const override;

        ASTNode getASTNode(TokenReader &tokenReader, const CPack *cpack = nullptr) const override;
    };

}// namespace CHelper::Node

CODEC_NODE_NONE(CHelper::Node::NodeCommandName)

#endif//CHELPER_NODECOMMANDNAME_H
