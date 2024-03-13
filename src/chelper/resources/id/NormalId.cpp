//
// Created by Yancey on 2023/11/7.
//

#include "NormalId.h"

namespace CHelper {

    NormalId::NormalId(const std::string &name,
                       const std::optional<std::string> &description)
            : name(name),
              description(description),
              nameHash(std::hash<std::string>{}(name)),
              mHashCode(HashUtil::combineHash(nameHash, description)) {}

    NormalId::NormalId(const nlohmann::json &j)
            : name(JsonUtil::fromJson<std::string>(j, "name")),
              description(JsonUtil::fromJsonOptional<std::string>(j, "description")),
              nameHash(std::hash<std::string>{}(name)),
              mHashCode(HashUtil::combineHash(nameHash, description)) {}

    void NormalId::toJson(nlohmann::json &j) const {
        JsonUtil::toJson(j, "name", name);
        JsonUtil::toJsonOptional(j, "description", description);
    }

} // CHelper