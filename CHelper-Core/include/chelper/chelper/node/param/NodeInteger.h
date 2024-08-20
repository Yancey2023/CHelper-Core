//
// Created by Yancey on 2023/11/11.
//

#pragma once

#ifndef CHELPER_NODEINTEGER_H
#define CHELPER_NODEINTEGER_H

#include "../NodeBase.h"
#include "../template/NodeNumber.h"

namespace CHelper::Node {

    class NodeInteger : public NodeNumber<int32_t> {
    public:
        NodeInteger() = default;

        [[nodiscard]] NodeType *getNodeType() const override;

        static std::unique_ptr<NodeInteger> make(const std::optional<std::u16string> &id,
                                                 const std::optional<std::u16string> &description,
                                                 const std::optional<int32_t> &min,
                                                 const std::optional<int32_t> &max);
    };

    CODEC_NODE_H(NodeInteger)

}// namespace CHelper::Node

#endif//CHELPER_NODEINTEGER_H
