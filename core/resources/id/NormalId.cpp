//
// Created by Yancey666 on 2023/11/7.
//

#include "NormalId.h"

namespace CHelper {

    NormalId::NormalId(std::string name,
                       const std::optional<std::string> &description)
            : name(std::move(name)),
              description(description) {}

    NormalId::NormalId(const nlohmann::json &j)
            : name(FROM_JSON(j, name, std::string)),
              description(FROM_JSON(j, description, std::string)) {}

    void NormalId::toJson(nlohmann::json &j) const {
        TO_JSON(j, name);
        TO_JSON_OPTIONAL(j, description);
    }

} // CHelper