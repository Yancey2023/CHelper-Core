//
// Created by Yancey on 2024/2/28.
//

#pragma once

#ifndef CHELPER_NODEJSONINTEGER_H
#define CHELPER_NODEJSONINTEGER_H

#include "../NodeBase.h"
#include "../template/NodeNumber.h"

namespace CHelper::Node {

    class NodeJsonInteger : public NodeNumber<int32_t> {
    public:
        NodeJsonInteger() = default;

        [[nodiscard]] NodeType *getNodeType() const override;

        static std::unique_ptr<NodeJsonInteger> make(const std::optional<std::u16string> &id,
                                                     const std::optional<std::u16string> &description,
                                                     const std::optional<int32_t> &min,
                                                     const std::optional<int32_t> &max);
    };

    CODEC_NODE_H(NodeJsonInteger)

}// namespace CHelper::Node

#endif//CHELPER_NODEJSONINTEGER_H
