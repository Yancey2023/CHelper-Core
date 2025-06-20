//
// Created by Yancey on 2024/3/1.
//

#pragma once

#ifndef CHELPER_NODEXPINTEGER_H
#define CHELPER_NODEXPINTEGER_H

#include "../NodeBase.h"
#include "../util/NodeAnd.h"
#include "../util/NodeOr.h"
#include "NodeNormalId.h"

namespace CHelper::Node {

    class NodeIntegerWithUnit : public NodeSerializable {
    public:
        std::shared_ptr<std::vector<std::shared_ptr<NormalId>>> units;
        std::unique_ptr<NodeNormalId> nodeUnits;
        std::unique_ptr<NodeAnd> nodeIntegerWithUnit;
        std::unique_ptr<NodeOr> nodeIntegerMaybeHaveUnit;

        NodeIntegerWithUnit() = default;

        void init(const CPack &cpack) override;

        [[nodiscard]] NodeTypeId::NodeTypeId getNodeType() const override;
    };

}// namespace CHelper::Node

CODEC_NODE(CHelper::Node::NodeIntegerWithUnit, units)

#endif//CHELPER_NODEXPINTEGER_H
