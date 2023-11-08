//
// Created by Yancey666 on 2023/11/7.
//

#ifndef CHELPER_MANIFEST_H
#define CHELPER_MANIFEST_H


#include <string>
#include <optional>
#include "nlohmann/json.hpp"
#include "id/NormalId.h"
#include "../util/JsonUtil.h"


class Manifest {
public:
    std::optional<std::string> name, description, minecraftVersion, author, updateDate;
    std::string packId;
    int version;
    std::optional<bool> isBasicPack, isDefault;

    Manifest(const std::optional<std::string> &name,
             const std::optional<std::string> &description,
             const std::optional<std::string> &minecraftVersion,
             const std::optional<std::string> &author,
             const std::optional<std::string> &updateDate,
             std::string packId,
             int version,
             const std::optional<bool> &isBasicPack,
             const std::optional<bool> &isDefault);

};

template<>
struct [[maybe_unused]] nlohmann::adl_serializer<Manifest> {

    static Manifest from_json(const nlohmann::json &j) {
        return {
                FROM_JSON_OPTIONAL(j, name, std::string),
                FROM_JSON_OPTIONAL(j, description, std::string),
                FROM_JSON_OPTIONAL(j, minecraftVersion, std::string),
                FROM_JSON_OPTIONAL(j, author, std::string),
                FROM_JSON_OPTIONAL(j, updateDate, std::string),
                FROM_JSON(j, packId, std::string),
                FROM_JSON(j, version, int),
                FROM_JSON_OPTIONAL(j, isBasicPack, bool),
                FROM_JSON_OPTIONAL(j, isDefault, bool)
        };
    }

    static void to_json(nlohmann::json &j, Manifest t) {
        TO_JSON_OPTIONAL(j, t, name)
        TO_JSON_OPTIONAL(j, t, description)
        TO_JSON_OPTIONAL(j, t, minecraftVersion)
        TO_JSON_OPTIONAL(j, t, author)
        TO_JSON_OPTIONAL(j, t, updateDate)
        TO_JSON(j, t, packId);
        TO_JSON(j, t, version);
        TO_JSON_OPTIONAL(j, t, isBasicPack)
        TO_JSON_OPTIONAL(j, t, isDefault)
    }
};

std::ostream &operator<<(std::ostream &os, const Manifest &manifest);

#endif //CHELPER_MANIFEST_H
