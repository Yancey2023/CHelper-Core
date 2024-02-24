//
// Created by Yancey on 2023/11/7.
//

#include "Manifest.h"

namespace CHelper {

    Manifest::Manifest(const std::optional<std::string> &name,
                       const std::optional<std::string> &description,
                       const std::optional<std::string> &minecraftVersion,
                       const std::optional<std::string> &author,
                       const std::optional<std::string> &updateDate,
                       std::string packId,
                       int version,
                       const std::optional<bool> &isBasicPack,
                       const std::optional<bool> &isDefault)
            : name(name),
              description(description),
              minecraftVersion(minecraftVersion),
              author(author),
              updateDate(updateDate),
              packId(std::move(packId)),
              version(version),
              isBasicPack(isBasicPack),
              isDefault(isDefault) {}

    Manifest::Manifest(const nlohmann::json &j)
            : name(FROM_JSON_OPTIONAL(j, name, std::string)),
              description(FROM_JSON_OPTIONAL(j, description, std::string)),
              minecraftVersion(FROM_JSON_OPTIONAL(j, minecraftVersion, std::string)),
              author(FROM_JSON_OPTIONAL(j, author, std::string)),
              updateDate(FROM_JSON_OPTIONAL(j, updateDate, std::string)),
              packId(FROM_JSON(j, packId, std::string)),
              version(FROM_JSON(j, version, int)),
              isBasicPack(FROM_JSON_OPTIONAL(j, isBasicPack, bool)),
              isDefault(FROM_JSON_OPTIONAL(j, isDefault, bool)) {}

    void Manifest::toJson(nlohmann::json &j) const {
        TO_JSON_OPTIONAL(j, name)
        TO_JSON_OPTIONAL(j, description)
        TO_JSON_OPTIONAL(j, minecraftVersion)
        TO_JSON_OPTIONAL(j, author)
        TO_JSON_OPTIONAL(j, updateDate)
        TO_JSON(j, packId);
        TO_JSON(j, version);
        TO_JSON_OPTIONAL(j, isBasicPack)
        TO_JSON_OPTIONAL(j, isDefault)
    }

}

std::ostream &operator<<(std::ostream &os, const CHelper::Manifest &manifest) {
    return os << (nlohmann::json) manifest;
}