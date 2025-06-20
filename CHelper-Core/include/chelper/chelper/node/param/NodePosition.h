//
// Created by Yancey on 2023/12/2.
//

#pragma once

#ifndef CHELPER_NODEPOSITION_H
#define CHELPER_NODEPOSITION_H

#include "../NodeBase.h"

namespace CHelper::Node {

    class NodePosition : public NodeSerializable {
    public:
        NodePosition(const std::optional<std::string> &id,
                     const std::optional<std::u16string> &description);

        NodePosition() = default;

        [[nodiscard]] NodeTypeId::NodeTypeId getNodeType() const override;

        ASTNode getASTNode(TokenReader &tokenReader, const CPack *cpack = nullptr) const override;
    };

}// namespace CHelper::Node

CODEC_NODE_NONE(CHelper::Node::NodePosition)

#endif//CHELPER_NODEPOSITION_H
