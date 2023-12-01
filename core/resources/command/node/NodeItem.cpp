//
// Created by Yancey666 on 2023/11/11.
//

#include "NodeItem.h"

namespace CHelper::Node {

    NodeItem::NodeItem(const std::optional<std::string> &id,
                       const std::optional<std::string> &description,
                       CHelper::Node::NodeItemType::NodeItemType nodeItemType)
            : NodeBase(NodeType::ITEM, id, description),
              nodeItemType(nodeItemType) {}

    NodeItem::NodeItem(const nlohmann::json &j) :
            NodeBase(NodeType::ITEM, j),
            nodeItemType(FROM_JSON(j, nodeItemType, CHelper::Node::NodeItemType::NodeItemType)) {}

    void NodeItem::toJson(nlohmann::json &j) const {
        NodeBase::toJson(j);
        j.push_back({"type", NodeType::STR_ITEM});
        TO_JSON(j, nodeItemType);
    }

} // CHelper::Node