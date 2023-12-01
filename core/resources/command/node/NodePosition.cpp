//
// Created by Yancey666 on 2023/12/2.
//

#include "NodePosition.h"

namespace CHelper {

    Node::NodePosition::NodePosition(Node::NodeType::NodeType type,
                                     const std::optional<std::string> &id,
                                     const std::optional<std::string> &description)
            : NodeBase(type, id, description) {}

    Node::NodePosition::NodePosition(const nlohmann::json &j)
            : NodeBase(NodeType::POSITION, j) {}

    void Node::NodePosition::toJson(nlohmann::json &j) const {
        NodeBase::toJson(j);
        j.push_back({"type", NodeType::STR_POSITION});
    }

} // CHelper