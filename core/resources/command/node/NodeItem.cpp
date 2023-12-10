//
// Created by Yancey666 on 2023/11/11.
//

#include "NodeItem.h"

namespace CHelper::Node {

    NODE_TYPE(ITEM, NodeItem);

    NodeItem::NodeItem(const std::optional<std::string> &id,
                       const std::optional<std::string> &description,
                       CHelper::Node::NodeItemType::NodeItemType nodeItemType)
            : NodeBase(id, description),
              nodeItemType(nodeItemType) {}

    NodeItem::NodeItem(const nlohmann::json &j,
                       const CPack &cpack) :
            NodeBase(j, cpack),
            nodeItemType(FROM_JSON(j, nodeItemType, CHelper::Node::NodeItemType::NodeItemType)) {}

    void NodeItem::toJson(nlohmann::json &j) const {
        NodeBase::toJson(j);
        TO_JSON(j, nodeItemType);
    }

} // CHelper::Node