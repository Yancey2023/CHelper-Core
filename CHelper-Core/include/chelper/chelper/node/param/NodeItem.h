//
// Created by Yancey on 2023/11/11.
//

#pragma once

#ifndef CHELPER_NODEITEM_H
#define CHELPER_NODEITEM_H

#include "../NodeBase.h"
#include "NodeNamespaceId.h"

namespace CHelper::Node {

    namespace NodeItemType {
        enum NodeItemType : uint8_t {
            // <物品ID> <物品数量> <附加值> [物品组件]
            ITEM_GIVE = 0,
            // <物品ID> <附加值> <物品数量>
            ITEM_CLEAR = 1
        };
    }// namespace NodeItemType

    class NodeItem : public NodeSerializable {
    public:
        static std::shared_ptr<NodeBase> nodeCount;
        static std::shared_ptr<NodeBase> nodeAllData;
        NodeItemType::NodeItemType nodeItemType = NodeItemType::ITEM_GIVE;
        std::unique_ptr<NodeBase> nodeItemId;
        std::shared_ptr<std::vector<std::shared_ptr<ItemId>>> itemIds;
        std::unique_ptr<NodeBase> nodeComponent;

        NodeItem() = default;

        void init(const CPack &cpack) override;

        [[nodiscard]] NodeTypeId::NodeTypeId getNodeType() const override;
    };

}// namespace CHelper::Node

CODEC_ENUM(CHelper::Node::NodeItemType::NodeItemType, uint8_t);

CODEC_NODE(CHelper::Node::NodeItem, nodeItemType)

#endif//CHELPER_NODEITEM_H
