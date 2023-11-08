//
// Created by Yancey666 on 2023/11/7.
//

#ifndef CHELPER_NORMALID_H
#define CHELPER_NORMALID_H

#include <string>
#include <optional>
#include "nlohmann/json.hpp"
#include "../../util/JsonUtil.h"

namespace CHelper {

    class NormalId {
    public:
        std::string name;
        std::optional<std::string> description;

        NormalId(std::string name, const std::optional<std::string> &description);
    };

}

template<>
struct [[maybe_unused]] nlohmann::adl_serializer<CHelper::NormalId> {

    static CHelper::NormalId from_json(const nlohmann::json &j) {
        return {
                FROM_JSON(j, name, std::string),
                FROM_JSON_OPTIONAL(j, description, std::string),
        };
    };

    static void to_json(nlohmann::json &j, const CHelper::NormalId &t) {
        TO_JSON(j, t, name);
        TO_JSON_OPTIONAL(j, t, description);
    };
};

#endif //CHELPER_NORMALID_H
