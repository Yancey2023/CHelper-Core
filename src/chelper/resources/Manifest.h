//
// Created by Yancey on 2023/11/7.
//

#ifndef CHELPER_MANIFEST_H
#define CHELPER_MANIFEST_H

#include "pch.h"
#include "id/NormalId.h"

namespace CHelper {

    class Manifest : public JsonUtil::ToJson {
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

        explicit Manifest(const nlohmann::json &j);

        void toJson(nlohmann::json &j) const override {
            TO_JSON_OPTIONAL(j, name)
            TO_JSON_OPTIONAL(j, description)
            TO_JSON_OPTIONAL(j, minecraftVersion)
            TO_JSON_OPTIONAL(j, author)
            TO_JSON_OPTIONAL(j, updateDate)
            TO_JSON(j, packId);
            TO_JSON(j, version);
            TO_JSON_OPTIONAL(j, isBasicPack)
            TO_JSON_OPTIONAL(j, isDefault)
        };

    };
}

CREATE_ADL_SERIALIZER(CHelper::Manifest);

std::ostream &operator<<(std::ostream &os, const CHelper::Manifest &manifest);

#endif //CHELPER_MANIFEST_H
