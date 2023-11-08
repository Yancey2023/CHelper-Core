//
// Created by Yancey666 on 2023/11/7.
//

#ifndef CHELPER_BLOCKID_H
#define CHELPER_BLOCKID_H


#include <optional>
#include <string>
#include <nlohmann/json.hpp>
#include "../../util/JsonUtil.h"

namespace CHelper {

    namespace BlockStateType {
        enum BlockStateType {
            STRING,
            BOOLEAN,
            INTEGER
        };
    }

    class BlockStateValue {
    public:
        BlockStateType::BlockStateType type;
        // std::string不是基本数据类型，不能在union中共享内存
        std::string valueString;
        union {
            bool valueBoolean{};
            int valueInteger;
        };
        std::optional<std::string> description;

        BlockStateValue(std::string value, const std::optional<std::string> &description);

        BlockStateValue(bool value, const std::optional<std::string> &description);

        BlockStateValue(int value, const std::optional<std::string> &description);


    };

    class BlockState {
    public:
        std::string key;
        std::optional<std::string> description;
        std::vector<BlockStateValue> values;
        int defaultValue;

        BlockState(std::string key,
                   const std::optional<std::string> &description,
                   const std::vector<BlockStateValue> &values,
                   int defaultValue);
    };

    class BlockId {
    public:
        std::optional<std::string> nameSpace;
        std::string name;
        std::optional<std::string> description;
        std::vector<BlockState> blockStates;
        std::optional<int> max;
        std::optional<std::vector<std::string>> descriptions;

        BlockId(const std::optional<std::string> &nameSpace,
                const std::string &name,
                const std::optional<std::string> &description,
                const std::vector<BlockState> &blockStateIds,
                const std::optional<int> &max,
                const std::optional<std::vector<std::string>> &descriptions);
    };

}

template<>
struct [[maybe_unused]] nlohmann::adl_serializer<CHelper::BlockStateValue> {

    static CHelper::BlockStateValue from_json(const nlohmann::json &j) {
        const nlohmann::json &name = j.at("name");
        std::optional<std::string> description = FROM_JSON_OPTIONAL(j, description, std::string);
        if (name.is_number_integer()) {
            return {name.get<int>(), description};
        } else if (name.is_boolean()) {
            return {name.get<bool>(), description};
        } else {
            return {name.get<std::string>(), description};
        }
    };

    static void to_json(nlohmann::json &j, CHelper::BlockStateValue t) {
        switch (t.type) {
            case CHelper::BlockStateType::STRING:
                TO_JSON(j, t, valueString);
                break;
            case CHelper::BlockStateType::BOOLEAN:
                TO_JSON(j, t, valueBoolean);
                break;
            case CHelper::BlockStateType::INTEGER:
                TO_JSON(j, t, valueInteger);
                break;
        }
        TO_JSON_OPTIONAL(j, t, description)
    };
};

template<>
struct [[maybe_unused]] nlohmann::adl_serializer<CHelper::BlockState> {

    static CHelper::BlockState from_json(const nlohmann::json &j) {
        return {
                FROM_JSON(j, key, std::string),
                FROM_JSON_OPTIONAL(j, description, std::string),
                FROM_JSON(j, values, std::vector<CHelper::BlockStateValue>),
                FROM_JSON(j, defaultValue, int)
        };
    };

    static void to_json(nlohmann::json &j, CHelper::BlockState t) {
        TO_JSON(j, t, key);
        TO_JSON_OPTIONAL(j, t, description)
        TO_JSON(j, t, values);
        TO_JSON(j, t, defaultValue);
    };
};

template<>
struct [[maybe_unused]] nlohmann::adl_serializer<CHelper::BlockId> {

    static CHelper::BlockId from_json(const nlohmann::json &j) {
        return {
                FROM_JSON_OPTIONAL(j, nameSpace, std::string),
                FROM_JSON(j, name, std::string),
                FROM_JSON_OPTIONAL(j, description, std::string),
                FROM_JSON(j, blockStates, std::vector<CHelper::BlockState>),
                FROM_JSON_OPTIONAL(j, max, int),
                FROM_JSON_OPTIONAL(j, descriptions, std::vector<std::string>)
        };
    };

    static void to_json(nlohmann::json &j, CHelper::BlockId t) {
        TO_JSON_OPTIONAL(j, t, nameSpace)
        TO_JSON(j, t, name);
        TO_JSON_OPTIONAL(j, t, description)
        TO_JSON(j, t, blockStates);
        TO_JSON_OPTIONAL(j, t, max)
        TO_JSON_OPTIONAL(j, t, descriptions)
    };
};

#endif //CHELPER_BLOCKID_H
