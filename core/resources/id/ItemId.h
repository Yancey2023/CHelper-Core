//
// Created by Yancey666 on 2023/11/8.
//

#ifndef CHELPER_ITEMID_H
#define CHELPER_ITEMID_H


#include <optional>
#include <string>
#include <vector>
#include "nlohmann/json.hpp"
#include "../../util/JsonUtil.h"

namespace CHelper {

    class ItemId {
    public:
        std::optional<std::string> nameSpace;
        std::string name;
        std::optional<std::string> description;
        std::optional<int> max;
        std::optional<std::vector<std::string>> descriptions;

    public:
        ItemId(const std::optional<std::string> &nameSpace,
               std::string name,
               const std::optional<std::string> &description,
               const std::optional<int> &max,
               const std::optional<std::vector<std::string>> &descriptions);
    };

}

template<>
struct [[maybe_unused]] nlohmann::adl_serializer<CHelper::ItemId> {

    static CHelper::ItemId from_json(const nlohmann::json &j) {
        return {
                FROM_JSON_OPTIONAL(j, nameSpace, std::string),
                FROM_JSON(j, name, std::string),
                FROM_JSON_OPTIONAL(j, description, std::string),
                FROM_JSON_OPTIONAL(j, max, int),
                FROM_JSON_OPTIONAL(j, descriptions, std::vector<std::string>)
        };
    };

    static void to_json(nlohmann::json &j, CHelper::ItemId t) {
        TO_JSON_OPTIONAL(j, t, nameSpace)
        TO_JSON(j, t, name);
        TO_JSON_OPTIONAL(j, t, description)
        TO_JSON_OPTIONAL(j, t, max)
        TO_JSON_OPTIONAL(j, t, descriptions)
    };
};

#endif //CHELPER_ITEMID_H
