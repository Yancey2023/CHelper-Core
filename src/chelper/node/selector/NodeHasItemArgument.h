//
// Created by Yancey on 2024/2/27.
//

#pragma once

#ifndef CHELPER_NODEHASITEMARGUMENT_H
#define CHELPER_NODEHASITEMARGUMENT_H

#include "../../resources/id/ItemId.h"
#include "../NodeBase.h"

namespace CHelper::Node {

    class NodeHasItemArgument : public NodeBase {
    private:
        const NodeBase *nodeSlot, *nodeItem;

    public:
        [[maybe_unused]] NodeHasItemArgument(const std::optional<std::string> &id,
                                             const std::optional<std::string> &description,
                                             const NodeBase *nodeItem,
                                             const NodeBase *nodeSlot);

        ASTNode getASTNode(TokenReader &tokenReader, const CPack *cpack) const override;
    };

}// namespace CHelper::Node

#endif//CHELPER_NODEHASITEMARGUMENT_H
