//
// Created by Yancey666 on 2023/11/11.
//

#ifndef CHELPER_NODEITEM_H
#define CHELPER_NODEITEM_H

#include "NodeBase.h"

namespace CHelper::Node {

    namespace NodeItemType {
        enum NodeItemType {
            // <物品ID> <附加值 | 方块状态>
            ITEM_NORMAL = 0,
            // <物品ID> <附加值>
            ITEM_ONLY_META_DATA = 1
        };
    }

    class NodeItem : public NodeBase {
    public:
        NodeItemType::NodeItemType nodeItemType;

        NodeItem(const std::optional<std::string> &id,
                 const std::optional<std::string> &description,
                 NodeItemType::NodeItemType nodeItemType);

        NodeItem(const nlohmann::json &j,
                 const CPack &cpack);

        NODE_TYPE_H;

        void toJson(nlohmann::json &j) const override;
    };

} // CHelper::Node

CREATE_ADL_SERIALIZER(CHelper::Node::NodeItem);

#endif //CHELPER_NODEITEM_H
