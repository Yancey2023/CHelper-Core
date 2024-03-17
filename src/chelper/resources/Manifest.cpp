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
                       int versionCode,
                       const std::optional<bool> &isBasicPack,
                       const std::optional<bool> &isDefault)
            : name(name),
              description(description),
              minecraftVersion(minecraftVersion),
              author(author),
              updateDate(updateDate),
              packId(std::move(packId)),
              versionCode(versionCode),
              isBasicPack(isBasicPack),
              isDefault(isDefault) {}

    Manifest::Manifest(const nlohmann::json &j)
            : name(JsonUtil::fromJsonOptionalLikely<std::string>(j, "name")),
              description(JsonUtil::fromJsonOptionalLikely<std::string>(j, "description")),
              minecraftVersion(JsonUtil::fromJsonOptionalLikely<std::string>(j, "minecraftVersion")),
              author(JsonUtil::fromJsonOptionalLikely<std::string>(j, "author")),
              updateDate(JsonUtil::fromJsonOptionalLikely<std::string>(j, "updateDate")),
              packId(JsonUtil::fromJson<std::string>(j, "packId")),
              versionCode(JsonUtil::fromJson<int>(j, "versionCode")),
              isBasicPack(JsonUtil::fromJsonOptionalLikely<bool>(j, "isBasicPack")),
              isDefault(JsonUtil::fromJsonOptionalLikely<bool>(j, "isDefault")) {}

    void Manifest::toJson(nlohmann::json &j) const {
        JsonUtil::toJsonOptionalLikely(j, "name", name);
        JsonUtil::toJsonOptionalLikely(j, "description", description);
        JsonUtil::toJsonOptionalLikely(j, "minecraftVersion", minecraftVersion);
        JsonUtil::toJsonOptionalLikely(j, "author", author);
        JsonUtil::toJsonOptionalLikely(j, "updateDate", updateDate);
        JsonUtil::toJson(j, "packId", packId);
        JsonUtil::toJson(j, "versionCode", versionCode);
        JsonUtil::toJsonOptionalLikely(j, "isBasicPack", isBasicPack);
        JsonUtil::toJsonOptionalLikely(j, "isDefault", isDefault);
    }

}

std::ostream &operator<<(std::ostream &os, const CHelper::Manifest &manifest) {
    return os << (nlohmann::json) manifest;
}