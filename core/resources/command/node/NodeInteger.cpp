//
// Created by Yancey666 on 2023/11/11.
//

#include "NodeInteger.h"

namespace CHelper::Node {

    NODE_TYPE(INTEGER, NodeInteger);

    NodeInteger::NodeInteger(const std::optional<std::string> &id,
                             const std::optional<std::string> &description,
                             const std::optional<int> &min,
                             const std::optional<int> &max)
            : NodeBase(id, description),
              min(min),
              max(max) {}

    NodeInteger::NodeInteger(const nlohmann::json &j,
                             const CPack &cpack)
            : NodeBase(j, cpack),
              min(FROM_JSON_OPTIONAL(j, min, int)),
              max(FROM_JSON_OPTIONAL(j, max, int)) {}

    void NodeInteger::toJson(nlohmann::json &j) const {
        NodeBase::toJson(j);
        TO_JSON_OPTIONAL(j, min)
        TO_JSON_OPTIONAL(j, max)
    }

} // CHelper::Node

