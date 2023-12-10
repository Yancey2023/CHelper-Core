//
// Created by Yancey666 on 2023/12/2.
//

#include "NodePosition.h"

namespace CHelper::Node {

    NODE_TYPE(POSITION, NodePosition);

    NodePosition::NodePosition(const std::optional<std::string> &id,
                               const std::optional<std::string> &description)
            : NodeBase(id, description) {}

    NodePosition::NodePosition(const nlohmann::json &j,
                               const CPack &cpack)
            : NodeBase(j, cpack) {}

} // CHelper::Node