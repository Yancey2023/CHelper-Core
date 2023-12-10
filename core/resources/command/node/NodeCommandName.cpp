//
// Created by Yancey666 on 2023/11/11.
//

#include "NodeCommandName.h"

namespace CHelper::Node {

    NODE_TYPE(COMMAND_NAME, NodeCommandName);

    NodeCommandName::NodeCommandName(const std::optional<std::string> &id,
                                     const std::optional<std::string> &description)
            : NodeBase(id, description) {}

    NodeCommandName::NodeCommandName(const nlohmann::json &j,
                                     const CPack &cpack)
            : NodeBase(j, cpack) {}

} // CHelper::Node