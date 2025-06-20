//
// Created by Yancey on 2024/2/28.
//

#pragma once

#ifndef CHELPER_NODEJSONLIST_H
#define CHELPER_NODEJSONLIST_H

#include "../NodeBase.h"

namespace CHelper::Node {

    class NodeJsonList : public NodeSerializable {
    public:
      static std::unique_ptr<NodeBase> nodeAllList;
        std::string data;
        std::unique_ptr<NodeBase> nodeList;

        NodeJsonList() = default;

        NodeJsonList(const std::optional<std::string> &id,
                     const std::optional<std::u16string> &description,
                     std::string data = std::string());

        using NodeBase::init;

        void init(const std::vector<std::unique_ptr<NodeSerializable>> &dataList);

        [[nodiscard]] NodeTypeId::NodeTypeId getNodeType() const override;
    };

}// namespace CHelper::Node

CODEC_NODE(CHelper::Node::NodeJsonList, data)

#endif//CHELPER_NODEJSONLIST_H
