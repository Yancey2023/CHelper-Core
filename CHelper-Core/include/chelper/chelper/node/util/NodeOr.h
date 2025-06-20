//
// Created by Yancey on 2023/12/22.
//

#pragma once

#ifndef CHELPER_NODEOR_H
#define CHELPER_NODEOR_H

#include "../NodeBase.h"

namespace CHelper::Node {

    class NodeOr : public NodeBase {
    public:
        std::vector<const NodeBase *> childNodes;
        bool isAttachToEnd = false, isUseFirst = false;
        bool noSuggestion = false;
        const char16_t *defaultErrorReason = nullptr;
        ASTNodeId::ASTNodeId nodeId = ASTNodeId::NONE;

        NodeOr() = default;

        NodeOr(std::vector<const NodeBase *> childNodes,
               bool isAttachToEnd,
               bool isUseFirst = false,
               bool noSuggestion = false,
               const char16_t *defaultErrorReason = nullptr,
               ASTNodeId::ASTNodeId nodeId = ASTNodeId::NONE);

        [[nodiscard]] NodeTypeId::NodeTypeId getNodeType() const override;
    };

}// namespace CHelper::Node


#endif//CHELPER_NODEOR_H
