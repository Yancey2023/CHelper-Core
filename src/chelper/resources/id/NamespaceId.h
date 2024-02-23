//
// Created by Yancey on 2023/11/11.
//

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

        void toJson(nlohmann::json &j) const override;

    };

} // CHelper

CREATE_ADL_SERIALIZER(CHelper::NamespaceId);

#endif //CHELPER_NAMESPACEID_H
