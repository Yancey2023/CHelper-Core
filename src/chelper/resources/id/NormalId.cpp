//
// Created by Yancey on 2023/11/7.
//

#include "NormalId.h"

namespace CHelper {

    void NormalId::buildHash() {
        if (HEDLEY_UNLIKELY(!isBuildHash)) {
            isBuildHash = true;
            nameHash = std::hash<std::wstring>{}(name);
            mHashCode = std::hash<std::wstring>{}(name);
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

    std::shared_ptr<NormalId> NormalId::make(const std::wstring &name, const std::optional<std::wstring> &description) {
        auto result = std::make_shared<NormalId>();
        result->name = name;
        result->description = description;
        return result;
    }

    CODEC(NormalId, name, description)

}// namespace CHelper