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
        std::variant<std::wstring, int32_t, bool> value;
        std::optional<std::wstring> description;

    private:
        std::shared_ptr<Node::NodeBase> node;

    public:
        std::shared_ptr<Node::NodeBase> getNode();
    };

    class BlockState {
    public:
        std::wstring key;
        std::optional<std::wstring> description;
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

    CODEC_H(BlockStateValue)

    CODEC_H(BlockState)

    CODEC_WITH_PARENT_H(BlockId)

}// namespace CHelper

#endif//CHELPER_BLOCKID_H
