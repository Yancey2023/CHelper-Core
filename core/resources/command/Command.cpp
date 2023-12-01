//
// Created by Yancey666 on 2023/11/10.
//

#include "Command.h"

namespace CHelper {

    Command::Command(const std::vector<std::string> &name,
                     const std::optional<std::string> &description,
                     const std::optional<std::vector<std::shared_ptr<Node::NodeBase>>> &startNodes)
            : name(name),
              description(description),
              startNodes(startNodes) {}

    void Command::addToSet(std::unordered_set<std::shared_ptr<Node::NodeBase>> &nodes) const {
        if (startNodes.has_value()) {
            for (const auto &startNode: startNodes.value()) {
                if (nodes.insert(startNode).second) {
                    startNode->addToSet(nodes);
                }
            }
        }
    }

    void Command::toJson(nlohmann::json &j) const {
        TO_JSON(j, name);
        TO_JSON_OPTIONAL(j, description)
        std::unordered_set<std::shared_ptr<Node::NodeBase>> node;
        addToSet(node);
        TO_JSON(j, node);
    }

} // CHelper