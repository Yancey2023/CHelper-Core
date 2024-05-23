//
// Created by Yancey on 2023/11/7.
//

#include "NormalId.h"

namespace CHelper {

    void NormalId::buildHash() {
        if (!isBuildHash) {
            isBuildHash = true;
            nameHash = std::hash<std::string>{}(name);
            mHashCode = std::hash<std::string>{}(name);
        }
    }

    [[nodiscard]] bool NormalId::fastMatch(size_t strHash) {
        buildHash();
        return nameHash == strHash;
    }

    [[nodiscard]] size_t NormalId::hashCode() {
        buildHash();
        return mHashCode;
    }

    std::shared_ptr<NormalId> NormalId::make(const std::string &name, const std::optional<std::string> &description) {
        auto result = std::make_shared<NormalId>();
        result->name = name;
        result->description = description;
        return result;
    }

    CODEC(NormalId, name, description);

}// namespace CHelper