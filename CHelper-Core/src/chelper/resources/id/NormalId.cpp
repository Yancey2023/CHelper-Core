//
// Created by Yancey on 2023/11/7.
//

#include <chelper/resources/id/NormalId.h>

namespace CHelper {

    void NormalId::buildHash() {
        if (!hashState.has_value()) {
            hashState = std::make_optional<XXH3_state_t>();
            XXH3_64bits_reset(&hashState.value());
            XXH3_64bits_update(&hashState.value(), name.data(), name.size() * sizeof(decltype(name)::value_type));
            nameHash = XXH3_64bits_digest(&hashState.value());
            if (description.has_value()) {
                XXH3_64bits_update(&hashState.value(), description.value().data(), description.value().size() * sizeof(decltype(description)::value_type::value_type));
            }
        }
    }

    [[nodiscard]] bool NormalId::fastMatch(XXH64_hash_t strHash) {
        buildHash();
        return nameHash == strHash;
    }

    [[nodiscard]] XXH3_state_t *NormalId::getHashState() {
        buildHash();
        return &hashState.value();
    }

    std::shared_ptr<NormalId> NormalId::make(const std::u16string &name, const std::optional<std::u16string> &description) {
        auto result = std::make_shared<NormalId>();
        result->name = name;
        result->description = description;
        return result;
    }

}// namespace CHelper