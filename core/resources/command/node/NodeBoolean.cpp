//
// Created by Yancey666 on 2023/11/11.
//

#include "NodeBoolean.h"

namespace CHelper::Node {

    NODE_TYPE(BOOLEAN, NodeBoolean)

    NodeBoolean::NodeBoolean(const std::optional<std::string> &id,
                             const std::optional<std::string> &description,
                             const std::optional<std::string> &descriptionTrue,
                             const std::optional<std::string> &descriptionFalse)
            : NodeBase(id, description),
              descriptionTrue(descriptionTrue),
              descriptionFalse(descriptionFalse) {}

    NodeBoolean::NodeBoolean(const nlohmann::json &j,
                             const CPack &cpack)
            : NodeBase(j, cpack),
              descriptionTrue(FROM_JSON_OPTIONAL(j, descriptionTrue, std::string)),
              descriptionFalse(FROM_JSON_OPTIONAL(j, descriptionFalse, std::string)) {}

    void NodeBoolean::toJson(nlohmann::json &j) const {
        NodeBase::toJson(j);
        TO_JSON_OPTIONAL(j, descriptionTrue)
        TO_JSON_OPTIONAL(j, descriptionFalse)
    }

} // CHelper::Node