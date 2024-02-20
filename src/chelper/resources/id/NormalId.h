//
// Created by Yancey666 on 2023/11/7.
//

#ifndef CHELPER_NORMALID_H
#define CHELPER_NORMALID_H

#include "pch.h"

namespace CHelper {

    class NormalId : public JsonUtil::ToJson {
    public:
        std::string name;
        std::optional<std::string> description;

        NormalId(std::string name, const std::optional<std::string> &description);

        explicit NormalId(const nlohmann::json &j);

        virtual ~NormalId() = default;

        void toJson(nlohmann::json &j) const override;
    };

} // CHelper

CREATE_ADL_SERIALIZER(CHelper::NormalId);

#endif //CHELPER_NORMALID_H
