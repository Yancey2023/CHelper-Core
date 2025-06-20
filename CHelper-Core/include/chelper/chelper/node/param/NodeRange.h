//
// Created by Yancey on 2024/2/27.
//

#pragma once

#ifndef CHELPER_NODERANGE_H
#define CHELPER_NODERANGE_H

#include "../NodeBase.h"

namespace CHelper::Node {

    class NodeRange : public NodeSerializable {
    public:
        NodeRange() = default;

        NodeRange(const std::optional<std::string> &id,
                  const std::optional<std::u16string> &description);

        [[nodiscard]] NodeTypeId::NodeTypeId getNodeType() const override;
    };

}// namespace CHelper::Node

CODEC_NODE_NONE(CHelper::Node::NodeRange)

#endif//CHELPER_NODERANGE_H
