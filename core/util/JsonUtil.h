//
// Created by Yancey666 on 2023/11/7.
//

#ifndef CHELPER_JSONUTIL_H
#define CHELPER_JSONUTIL_H

#include "nlohmann/json.hpp"

#define FROM_JSON(json, name, type) json.at(#name).get<type>()

#define FROM_JSON_OPTIONAL(json, name, type) json.contains(#name) ? std::optional<type>(json.at(#name).get<type>()) : std::nullopt

#define TO_JSON(json, target, name) json.push_back({#name, target.name})

#define TO_JSON_OPTIONAL(json, target, name) if (target.name.has_value()) {json.push_back({#name, target.name.value()});}

namespace CHelper::JsonUtil {

    nlohmann::json getJsonFromPath(const std::filesystem::path &path);

}

#endif //CHELPER_JSONUTIL_H
