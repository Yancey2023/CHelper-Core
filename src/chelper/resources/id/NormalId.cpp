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
            : name(FROM_JSON(j, name, std::string)),
              description(FROM_JSON_OPTIONAL(j, description, std::string)),
              nameHash(std::hash<std::string>{}(name)),
              mHashCode(HashUtil::combineHash(nameHash, description)) {}

    void NormalId::toJson(nlohmann::json &j) const {
        TO_JSON(j, name);
        TO_JSON_OPTIONAL(j, description)
    }

} // CHelper