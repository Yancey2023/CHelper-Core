//
// Created by Yancey on 2023/11/7.
//

#pragma once

#ifndef CHELPER_NORMALID_H
#define CHELPER_NORMALID_H

#include "pch.h"

namespace CHelper {

    class NormalId {
    public:
        std::string name;
        std::optional<std::string> description;
    private:
        const size_t nameHash, mHashCode;

    public:
        NormalId(const std::string &name, const std::optional<std::string> &description);

        explicit NormalId(const nlohmann::json &j);

        virtual ~NormalId() = default;

        NormalId(const NormalId &) = delete;

        NormalId &operator=(const NormalId &) = delete;

        virtual void toJson(nlohmann::json &j) const;

        [[nodiscard]] inline bool fastMatch(size_t strHash) const {
            return nameHash == strHash;
        }

        [[nodiscard]] inline size_t hashCode() const {
            return mHashCode;
        }

        [[nodiscard]] bool equal(const std::shared_ptr<NormalId> &id) const {
            return id.get() == this || mHashCode == id->mHashCode;
        }

    };

} // CHelper

template<>
struct nlohmann::adl_serializer<CHelper::NormalId> {

    static CHelper::NormalId from_json(const nlohmann::json &j) { return CHelper::NormalId(j); }

    static void to_json(nlohmann::json &j, const CHelper::NormalId &t) { t.toJson(j); }

};


#endif //CHELPER_NORMALID_H
