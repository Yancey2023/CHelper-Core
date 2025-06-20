//
// Created by Yancey on 2024/2/28.
//

#pragma once

#ifndef CHELPER_NODEJSON_H
#define CHELPER_NODEJSON_H

#include "../NodeBase.h"

namespace CHelper::Node {

    class NodeJson : public NodeSerializable {
    public:
        std::string key;
        const NodeBase *nodeJson = nullptr;

        NodeJson(const std::optional<std::string> &id,
                 const std::optional<std::u16string> &description,
                 std::string key);

        NodeJson() = default;

        void init(const CPack &cpack) override;

        [[nodiscard]] NodeTypeId::NodeTypeId getNodeType() const override;
    };

}// namespace CHelper::Node

CODEC_NODE(CHelper::Node::NodeJson, key)

#endif//CHELPER_NODEJSON_H
