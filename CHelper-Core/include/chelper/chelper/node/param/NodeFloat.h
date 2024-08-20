//
// Created by Yancey on 2023/11/11.
//

#pragma once

#ifndef CHELPER_NODEFLOAT_H
#define CHELPER_NODEFLOAT_H

#include "../NodeBase.h"
#include "../template/NodeNumber.h"

namespace CHelper::Node {

    class NodeFloat : public NodeNumber<float> {
    public:
        NodeFloat() = default;

        [[nodiscard]] NodeType *getNodeType() const override;

        static std::unique_ptr<NodeFloat> make(const std::optional<std::u16string> &id,
                                               const std::optional<std::u16string> &description,
                                               const std::optional<float> &min,
                                               const std::optional<float> &max);
    };

    CODEC_NODE_H(NodeFloat)

}// namespace CHelper::Node

#endif//CHELPER_NODEFLOAT_H
