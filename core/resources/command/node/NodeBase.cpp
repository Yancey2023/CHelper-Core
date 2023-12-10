//
// Created by Yancey666 on 2023/11/10.
//

#include "NodeBase.h"
#include "NodeLF.h"

namespace CHelper::Node {

    NodeType NodeBase::TYPE("UNKNOWN", [](const nlohmann::json &j, const CPack &cpack) {
        return std::shared_ptr<NodeBase>();
    });

    NodeBase::NodeBase(const std::optional<std::string> &id,
                       const std::optional<std::string> &description)
            : id(id),
              description(description) {}

    NodeBase::NodeBase(const nlohmann::json &j,
                       const CPack &cpack)
            : id(FROM_JSON_OPTIONAL(j, id, std::string)),
              description(FROM_JSON_OPTIONAL(j, description, std::string)) {}

    NodeType NodeBase::getNodeType() const {
        return NodeBase::TYPE;
    }

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
        j.push_back({"type", getNodeType().nodeName});
    }

} // CHelper::Node
