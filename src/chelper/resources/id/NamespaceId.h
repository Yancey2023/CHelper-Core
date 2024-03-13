//
// Created by Yancey on 2023/11/11.
//

#pragma once

#ifndef CHELPER_NAMESPACEID_H
#define CHELPER_NAMESPACEID_H

#include "pch.h"
#include "NormalId.h"

namespace CHelper {

    class NamespaceId : public NormalId {
    public:
        std::optional<std::string> nameSpace;
        std::shared_ptr<NormalId> idWithNamespace;

        NamespaceId(const std::optional<std::string> &nameSpace,
                    const std::string &name,
                    const std::optional<std::string> &description);

        explicit NamespaceId(const nlohmann::json &j);

        NamespaceId(const NamespaceId &) = delete;

        NamespaceId &operator=(const NamespaceId &) = delete;

        void toJson(nlohmann::json &j) const override;

    };

} // CHelper

template<>
struct nlohmann::adl_serializer<CHelper::NamespaceId> {

    static CHelper::NamespaceId from_json(const nlohmann::json &j) { return CHelper::NamespaceId(j); }

    static void to_json(nlohmann::json &j, const CHelper::NamespaceId &t) { t.toJson(j); }

};

#endif //CHELPER_NAMESPACEID_H
