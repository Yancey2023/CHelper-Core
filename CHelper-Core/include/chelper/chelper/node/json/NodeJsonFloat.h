//
// Created by Yancey on 2024/2/28.
//

#pragma once

#ifndef CHELPER_NODEJSONFLOAT_H
#define CHELPER_NODEJSONFLOAT_H

#include "../NodeBase.h"
#include "../template/NodeNumber.h"

namespace CHelper::Node {

    class NodeJsonFloat : public NodeNumber<float> {
    public:
        NodeJsonFloat() = default;

        [[nodiscard]] NodeType *getNodeType() const override;

        static std::unique_ptr<NodeJsonFloat> make(const std::optional<std::u16string> &id,
                                                   const std::optional<std::u16string> &description,
                                                   const std::optional<float> &min,
                                                   const std::optional<float> &max);
    };

    CODEC_NODE_H(NodeJsonFloat)

}// namespace CHelper::Node

#endif//CHELPER_NODEJSONFLOAT_H
