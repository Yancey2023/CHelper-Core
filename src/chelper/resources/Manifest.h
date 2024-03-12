//
// Created by Yancey on 2023/11/7.
//

#ifndef CHELPER_MANIFEST_H
#define CHELPER_MANIFEST_H

#include "pch.h"
#include "id/NormalId.h"

namespace CHelper {

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

        explicit Manifest(const nlohmann::json &j);

        Manifest(const Manifest &) = delete;

        Manifest &operator=(const Manifest &) = delete;

        void toJson(nlohmann::json &j) const;

    };
}

CREATE_ADL_SERIALIZER(CHelper::Manifest);

std::ostream &operator<<(std::ostream &os, const CHelper::Manifest &manifest);

#endif //CHELPER_MANIFEST_H
