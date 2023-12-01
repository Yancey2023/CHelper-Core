//
// Created by Yancey666 on 2023/11/10.
//

#include "NodeBase.h"
#include "NodeLF.h"

namespace CHelper::Node {

    NodeBase::NodeBase(NodeType::NodeType type,
                       const std::optional<std::string> &id,
                       const std::optional<std::string> &description)
            : type(type),
              id(id),
              description(description) {}

    NodeBase::NodeBase(NodeType::NodeType type,
                       const nlohmann::json &j)
            : type(type),
              id(FROM_JSON_OPTIONAL(j, id, std::string)),
              description(FROM_JSON_OPTIONAL(j, description, std::string)) {}

    void NodeBase::addToSet(std::unordered_set<std::shared_ptr<NodeBase>> &nodes) {
        if (!nextNodes.empty()) {
            for (const auto &nextNode: nextNodes) {
                if (nextNode == NodeLF::getInstance()) {
                    continue;
                }
                if (nodes.insert(nextNode).second) {
                    nextNode->addToSet(nodes);
                }
            }
        }
    }

    void NodeBase::toJson(nlohmann::json &j) const {
        TO_JSON_OPTIONAL(j, id)
        TO_JSON_OPTIONAL(j, description)
    }

} // CHelper::Node
