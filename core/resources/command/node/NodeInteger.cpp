//
// Created by Yancey666 on 2023/11/11.
//

#include "NodeInteger.h"

namespace CHelper::Node {

    NodeInteger::NodeInteger(const std::optional<std::string> &id,
                             const std::optional<std::string> &description,
                             const std::optional<int> &min,
                             const std::optional<int> &max)
            : NodeBase(NodeType::INTEGER, id, description),
              min(min),
              max(max) {}

    NodeInteger::NodeInteger(const nlohmann::json &j)
            : NodeBase(NodeType::INTEGER, j),
              min(FROM_JSON_OPTIONAL(j, min, int)),
              max(FROM_JSON_OPTIONAL(j, max, int)) {}

    void NodeInteger::toJson(nlohmann::json &j) const {
        NodeBase::toJson(j);
        j.push_back({"type", NodeType::STR_INTEGER});
        TO_JSON_OPTIONAL(j, min)
        TO_JSON_OPTIONAL(j, max)
    }

} // CHelper::Node

