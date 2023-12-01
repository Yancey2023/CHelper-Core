//
// Created by Yancey666 on 2023/11/11.
//

#include "NodeCommandName.h"

namespace CHelper::Node {

    NodeCommandName::NodeCommandName(const std::optional<std::string> &id,
                                     const std::optional<std::string> &description)
            : NodeBase(NodeType::COMMAND_NAME, id, description) {}

    NodeCommandName::NodeCommandName(const nlohmann::json &j)
            : NodeBase(NodeType::COMMAND_NAME, j) {}

    void NodeCommandName::toJson(nlohmann::json &j) const {
        NodeBase::toJson(j);
        j.push_back({"type", NodeType::STR_COMMAND_NAME});
    }

} // CHelper::Node