//
// Created by Yancey on 2023/11/7.
//

#pragma once

#ifndef CHELPER_BLOCKID_H
#define CHELPER_BLOCKID_H

#include "ItemId.h"
#include "pch.h"

namespace CHelper {

    namespace BlockStateType {
        enum BlockStateType : uint8_t {
            STRING,
            BOOLEAN,
            INTEGER
        };
    }

    class BlockStateValue {
    public:
        BlockStateType::BlockStateType type;
        std::variant<std::string, int32_t, bool> value;
        std::optional<std::string> description;

    private:
        std::shared_ptr<Node::NodeBase> node;

    public:
        std::shared_ptr<Node::NodeBase> getNode();
    };

    void from_json(const nlohmann::json &j, BlockStateValue &t);

    void to_json(nlohmann::json &j, const BlockStateValue &t);

    void from_binary(BinaryReader &binaryReader, BlockStateValue &t);

    void to_binary(BinaryWriter &binaryWriter, const BlockStateValue &t);

    class BlockState {
    public:
        std::string key;
        std::optional<std::string> description;
        std::vector<BlockStateValue> values;
        int32_t defaultValue;

    private:
        std::vector<std::shared_ptr<Node::NodeBase>> nodeChildren;
        std::shared_ptr<Node::NodeBase> node;

    public:
        std::shared_ptr<Node::NodeBase> getNode();
    };

    class BlockId : public NamespaceId {
    public:
        std::optional<std::vector<BlockState>> blockStates;

    private:
        std::vector<std::shared_ptr<Node::NodeBase>> nodeChildren;
        std::shared_ptr<Node::NodeBase> node;

    public:
        std::shared_ptr<Node::NodeBase> getNode();

        static Node::NodeBase *getNodeAllBlockState();
    };

    CODEC_H(BlockState);

    CODEC_H(BlockId);

}// namespace CHelper

#endif//CHELPER_BLOCKID_H
