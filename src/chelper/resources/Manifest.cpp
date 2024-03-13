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
            : name(JsonUtil::fromJsonOptional<std::string>(j, "name")),
              description(JsonUtil::fromJsonOptional<std::string>(j, "description")),
              minecraftVersion(JsonUtil::fromJsonOptional<std::string>(j, "minecraftVersion")),
              author(JsonUtil::fromJsonOptional<std::string>(j, "author")),
              updateDate(JsonUtil::fromJsonOptional<std::string>(j, "updateDate")),
              packId(JsonUtil::fromJson<std::string>(j, "packId")),
              version(JsonUtil::fromJson<int>(j, "version")),
              isBasicPack(JsonUtil::fromJsonOptional<bool>(j, "isBasicPack")),
              isDefault(JsonUtil::fromJsonOptional<bool>(j, "isDefault")) {}

    void Manifest::toJson(nlohmann::json &j) const {
        JsonUtil::toJsonOptional(j, "name", name);
        JsonUtil::toJsonOptional(j, "description", description);
        JsonUtil::toJsonOptional(j, "minecraftVersion", minecraftVersion);
        JsonUtil::toJsonOptional(j, "author", author);
        JsonUtil::toJsonOptional(j, "updateDate", updateDate);
        JsonUtil::toJson(j, "packId", packId);
        JsonUtil::toJson(j, "version", version);
        JsonUtil::toJsonOptional(j, "isBasicPack", isBasicPack);
        JsonUtil::toJsonOptional(j, "isDefault", isDefault);
    }

}

std::ostream &operator<<(std::ostream &os, const CHelper::Manifest &manifest) {
    return os << (nlohmann::json) manifest;
}