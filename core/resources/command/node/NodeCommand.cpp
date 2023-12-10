//
// Created by Yancey666 on 2023/11/11.
//

#include "NodeCommand.h"

namespace CHelper::Node {

    NODE_TYPE(COMMAND, NodeCommand);

    NodeCommand::NodeCommand(const std::optional<std::string> &id,
                             const std::optional<std::string> &description)
            : NodeBase(id, description) {}

    NodeCommand::NodeCommand(const nlohmann::json &j,
                             const CPack &cpack)
            : NodeBase(j, cpack) {}

} // CHelper::Node