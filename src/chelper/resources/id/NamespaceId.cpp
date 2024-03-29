//
// Created by Yancey on 2023/11/11.
//

#include "NamespaceId.h"

namespace CHelper {

    NamespaceId::NamespaceId(const std::optional<std::string> &nameSpace,
                             const std::string &name,
                             const std::optional<std::string> &description)
            : NormalId(name, description),
              nameSpace(nameSpace),
              idWithNamespace(std::make_shared<NormalId>(std::string(nameSpace.value_or("minecraft"))
                                                                 .append(":").append(name), description)) {}

    NamespaceId::NamespaceId(const nlohmann::json &j)
            : NormalId(j),
              nameSpace(JsonUtil::fromJsonOptionalUnlikely<std::string>(j, "nameSpace")),
              idWithNamespace(std::make_shared<NormalId>(std::string(nameSpace.value_or("minecraft")).
                      append(":").append(name), description)) {}

    void NamespaceId::toJson(nlohmann::json &j) const {
        NormalId::toJson(j);
        JsonUtil::toJsonOptionalUnlikely(j, "namespace", nameSpace);
    }

} // CHelper