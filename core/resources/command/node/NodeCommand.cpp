//
// Created by Yancey666 on 2023/11/11.
//

#include "NodeCommand.h"

namespace CHelper::Node {

    NodeCommand::NodeCommand(const std::optional<std::string> &id,
                             const std::optional<std::string> &description)
            : NodeBase(NodeType::COMMAND, id, description) {}

    NodeCommand::NodeCommand(const nlohmann::json &j)
            : NodeBase(NodeType::COMMAND, j) {}

    void NodeCommand::toJson(nlohmann::json &j) const {
        NodeBase::toJson(j);
        j.push_back({"type", NodeType::STR_COMMAND});
    }

} // CHelper::Node