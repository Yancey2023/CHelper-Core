//
// Created by Yancey on 2023/11/7.
//

#include "BlockId.h"

namespace CHelper {

    BlockStateValue::BlockStateValue(BlockStateType::BlockStateType type,
                                     const std::variant<std::string, int, bool> &value,
                                     const std::optional<std::string> &description)
            : type(type),
              value(value),
              description(description) {}

    BlockStateValue::BlockStateValue(const nlohmann::json &j)
            : description(FROM_JSON_OPTIONAL(j, description, std::string)) {
        const nlohmann::json &jsonValue = j.at("value");
        if (jsonValue.is_number_integer()) {
            type = CHelper::BlockStateType::INTEGER;
            value = jsonValue.get<int>();
        } else if (jsonValue.is_boolean()) {
            type = CHelper::BlockStateType::BOOLEAN;
            value = jsonValue.get<bool>();
        } else {
            type = CHelper::BlockStateType::STRING;
            value = jsonValue.get<std::string>();
        }
    }

    void BlockStateValue::toJson(nlohmann::json &j) const {
        switch (type) {
            case CHelper::BlockStateType::STRING:
                j["value"] = std::get<std::string>(value);
                break;
            case CHelper::BlockStateType::BOOLEAN:
                j["value"] = std::get<bool>(value);
                break;
            case CHelper::BlockStateType::INTEGER:
                j["value"] = std::get<int>(value);
                break;
        }
        TO_JSON_OPTIONAL(j, description)
    }

    BlockState::BlockState(std::string key,
                           const std::optional<std::string> &description,
                           const std::vector<BlockStateValue> &values,
                           int defaultValue)
            : key(std::move(key)),
              description(description),
              values(values),
              defaultValue(defaultValue) {}

    BlockState::BlockState(const nlohmann::json &j)
            : key(FROM_JSON(j, key, std::string)),
              description(FROM_JSON_OPTIONAL(j, description, std::string)),
              values(FROM_JSON(j, values, std::vector<CHelper::BlockStateValue>)),
              defaultValue(FROM_JSON(j, defaultValue, int)) {}

    void BlockState::toJson(nlohmann::json &j) const {
        TO_JSON(j, key);
        TO_JSON_OPTIONAL(j, description)
        TO_JSON(j, values);
        TO_JSON(j, defaultValue);
    }

    BlockId::BlockId(const std::optional<std::string> &nameSpace,
                     const std::string &name,
                     const std::optional<std::string> &description,
                     const std::optional<int> &max,
                     const std::optional<std::vector<std::string>> &descriptions,
                     const std::optional<std::vector<BlockState>> &blockStates)
            : ItemId(nameSpace, name, description, max, descriptions),
              blockStates(blockStates) {}

    BlockId::BlockId(const nlohmann::json &j)
            : ItemId(j),
              blockStates(FROM_JSON_OPTIONAL(j, blockStates, std::vector<CHelper::BlockState>)) {}

    void BlockId::toJson(nlohmann::json &j) const {
        TO_JSON_OPTIONAL(j, nameSpace)
        TO_JSON(j, name);
        TO_JSON_OPTIONAL(j, description)
        TO_JSON_OPTIONAL(j, blockStates)
        TO_JSON_OPTIONAL(j, max)
        TO_JSON_OPTIONAL(j, descriptions)
    }

} // CHelper