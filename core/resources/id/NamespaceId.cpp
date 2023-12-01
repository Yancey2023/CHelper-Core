//
// Created by Yancey666 on 2023/11/11.
//

#include "NamespaceId.h"

namespace CHelper {

    NamespaceId::NamespaceId(const std::optional<std::string> &nameSpace,
                             const std::string &name,
                             const std::optional<std::string> &description)
            : nameSpace(nameSpace),
              NormalId(name, description) {}

    NamespaceId::NamespaceId(const nlohmann::json &j)
            : NormalId(j),
              nameSpace(FROM_JSON_OPTIONAL(j, nameSpace, std::string)) {}

    void NamespaceId::toJson(nlohmann::json &j) const {
        NormalId::toJson(j);
        TO_JSON_OPTIONAL(j, nameSpace)
    }

} // CHelper