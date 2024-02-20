//
// Created by Yancey666 on 2023/11/7.
//

#ifndef CHELPER_BLOCKID_H
#define CHELPER_BLOCKID_H

#include "pch.h"
#include "ItemId.h"

namespace CHelper {

    namespace BlockStateType {
        enum BlockStateType {
            STRING,
            BOOLEAN,
            INTEGER
        };
    }

    class BlockStateValue : public JsonUtil::ToJson {
    public:
        BlockStateType::BlockStateType type;
        std::variant<std::string, int, bool> value;
        std::optional<std::string> description;

        BlockStateValue(BlockStateType::BlockStateType type,
                        const std::variant<std::string, int, bool> &value,
                        const std::optional<std::string> &description);

        explicit BlockStateValue(const nlohmann::json &j);

        void toJson(nlohmann::json &j) const override;

    };

    class BlockState : public JsonUtil::ToJson {
    public:
        std::string key;
        std::optional<std::string> description;
        std::vector<BlockStateValue> values;
        int defaultValue;

        BlockState(std::string key,
                   const std::optional<std::string> &description,
                   const std::vector<BlockStateValue> &values,
                   int defaultValue);

        explicit BlockState(const nlohmann::json &j);

        void toJson(nlohmann::json &j) const override;
    };

    class BlockId : public ItemId {
    public:
        std::optional<std::vector<BlockState>> blockStates;

        BlockId(const std::optional<std::string> &nameSpace,
                const std::string &name,
                const std::optional<std::string> &description,
                const std::optional<int> &max,
                const std::optional<std::vector<std::string>> &descriptions,
                const std::optional<std::vector<BlockState>> &blockStates);

        explicit BlockId(const nlohmann::json &j);

        void toJson(nlohmann::json &j) const override;
    };

} // CHelper

CREATE_ADL_SERIALIZER(CHelper::BlockStateValue);

CREATE_ADL_SERIALIZER(CHelper::BlockState);

CREATE_ADL_SERIALIZER(CHelper::BlockId);

#endif //CHELPER_BLOCKID_H
