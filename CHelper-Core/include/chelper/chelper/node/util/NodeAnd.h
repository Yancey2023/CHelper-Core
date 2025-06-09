//
// Created by Yancey on 2023/12/22.
//

#pragma once

#ifndef CHELPER_NODEAND_H
#define CHELPER_NODEAND_H

#include "../NodeBase.h"

namespace CHelper::Node {

    class NodeAnd : public NodeBase {
    public:
        std::vector<const NodeBase *> childNodes;

        NodeAnd() = default;

        NodeAnd(const std::optional<std::string> &id,
                const std::optional<std::u16string> &description,
                const std::vector<const NodeBase *> &childNodes);

        [[nodiscard]] NodeTypeId::NodeTypeId getNodeType() const override;

        ASTNode getASTNode(TokenReader &tokenReader, const CPack *cpack, void *private_data = nullptr) const override;

        std::optional<std::u16string> collectDescription(const ASTNode *node, size_t index) const override;
    };

}// namespace CHelper::Node

#endif//CHELPER_NODEAND_H
