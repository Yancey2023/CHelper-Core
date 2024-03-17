//
// Created by Yancey on 2023/11/7.
//

#pragma once

#ifndef CHELPER_BLOCKID_H
#define CHELPER_BLOCKID_H

#include "pch.h"
#include "ItemId.h"

namespace CHelper {

    namespace BlockStateType {
        enum BlockStateType : std::uint8_t {
            STRING,
            BOOLEAN,
            INTEGER
        };
    }

    class BlockStateValue {
    public:
        BlockStateType::BlockStateType type;
        std::variant<std::string, int, bool> value;
        std::optional<std::string> description;
        Node::NodeBase *node;

        BlockStateValue(BlockStateType::BlockStateType type,
                        const std::variant<std::string, int, bool> &value,
                        const std::optional<std::string> &description);

        explicit BlockStateValue(const nlohmann::json &j);

        BlockStateValue(const BlockStateValue &blockStateValue);

        ~BlockStateValue();

        BlockStateValue &operator=(const BlockStateValue &blockStateValue);

        void toJson(nlohmann::json &j) const;

    };

    class BlockState {
    public:
        std::string key;
        std::optional<std::string> description;
        std::vector<BlockStateValue> values;
        int defaultValue;
        Node::NodeBase *node;

        BlockState(std::string key,
                   const std::optional<std::string> &description,
                   std::vector<BlockStateValue> values,
                   int defaultValue);

        explicit BlockState(const nlohmann::json &j);

        BlockState(const BlockState &blockState);

        ~BlockState();

        BlockState &operator=(const BlockState &blockState);

        void toJson(nlohmann::json &j) const;
    };

    class BlockId : public ItemId {
    public:
        std::optional<std::vector<BlockState>> blockStates;
        Node::NodeBase *nodeBlockState;

        BlockId(const std::optional<std::string> &nameSpace,
                const std::string &name,
                const std::optional<std::string> &description,
                const std::optional<int> &max,
                const std::optional<std::vector<std::string>> &descriptions,
                const std::optional<std::vector<BlockState>> &blockStates);

        explicit BlockId(const nlohmann::json &j);

        ~BlockId() override;

        BlockId(const BlockId &);

        BlockId &operator=(const BlockId &blockId) = delete;

        void toJson(nlohmann::json &j) const override;

        static Node::NodeBase *getNodeAllBlockState();
    };

} // CHelper

template<>
struct [[maybe_unused]] nlohmann::adl_serializer<CHelper::BlockStateValue> {

    static CHelper::BlockStateValue from_json(const nlohmann::json &j) {
        return CHelper::BlockStateValue(j);
    }

    static void to_json(nlohmann::json &j, const CHelper::BlockStateValue &t) {
        t.toJson(j);
    }

};

template<>
struct [[maybe_unused]] nlohmann::adl_serializer<CHelper::BlockState> {

    static CHelper::BlockState from_json(const nlohmann::json &j) {
        return CHelper::BlockState(j);
    }

    static void to_json(nlohmann::json &j, const CHelper::BlockState &t) {
        t.toJson(j);
    }

};

template<>
struct [[maybe_unused]] nlohmann::adl_serializer<CHelper::BlockId> {

    static CHelper::BlockId from_json(const nlohmann::json &j) {
        return CHelper::BlockId(j);
    }

    static void to_json(nlohmann::json &j, const CHelper::BlockId &t) {
        t.toJson(j);
    }

};

template<>
struct [[maybe_unused]] nlohmann::adl_serializer<std::shared_ptr<CHelper::BlockId>> {

    static std::shared_ptr<CHelper::BlockId> from_json(const nlohmann::json &j) {
        return std::make_shared<CHelper::BlockId>(j);
    }

    static void to_json(nlohmann::json &j, const std::shared_ptr<CHelper::BlockId> &t) {
        t->toJson(j);
    }

};

#endif //CHELPER_BLOCKID_H
