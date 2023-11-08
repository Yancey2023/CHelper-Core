//
// Created by Yancey666 on 2023/11/7.
//

#include "Manifest.h"

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

std::ostream &operator<<(std::ostream &os, const Manifest &manifest) {
    return os << (nlohmann::json) manifest;
}