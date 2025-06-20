//
// Created by Yancey on 2023/12/10.
//

#pragma once

#ifndef CHELPER_NODESTRING_H
#define CHELPER_NODESTRING_H

#include "../NodeBase.h"
#include "pch.h"

namespace CHelper::Node {

    class NodeString : public NodeSerializable {
    public:
        bool allowMissingString = false;
        bool canContainSpace = false;
        bool ignoreLater = false;

        NodeString() = default;

        NodeString(const std::optional<std::string> &id,
                   const std::optional<std::u16string> &description,
                   bool allowMissingString,
                   bool canContainSpace,
                   bool ignoreLater);

        [[nodiscard]] NodeTypeId::NodeTypeId getNodeType() const override;

        ASTNode getASTNode(TokenReader &tokenReader, const CPack *cpack = nullptr) const override;
    };

}// namespace CHelper::Node

CODEC_NODE(CHelper::Node::NodeString, canContainSpace, ignoreLater)

#endif//CHELPER_NODESTRING_H
