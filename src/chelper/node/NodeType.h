//
// Created by Yancey on 2023/12/10.
//

#pragma once

#ifndef CHELPER_NODETYPE_H
#define CHELPER_NODETYPE_H

#include "pch.h"

namespace CHelper {

    class CPack;

    namespace Node {

        class NodeBase;

        class NodeType {
        public:
            uint8_t id{};
            std::string nodeName;
            std::function<void(const nlohmann::json &j, std::unique_ptr<NodeBase> &t)> decodeByJson;
            std::function<void(nlohmann::json &j, const std::unique_ptr<NodeBase> &t)> encodeByJson;
            std::function<void(BinaryReader &binaryReader, std::unique_ptr<NodeBase> &t)> decodeByBinary;
            std::function<void(BinaryWriter &binaryWriter, const std::unique_ptr<NodeBase> &t)> encodeByBinary;

            NodeType(std::string nodeName,
                     std::function<void(const nlohmann::json &j, std::unique_ptr<NodeBase> &t)> decodeByJson,
                     std::function<void(nlohmann::json &j, const std::unique_ptr<NodeBase> &t)> encodeByJson,
                     std::function<void(BinaryReader &binaryReader, std::unique_ptr<NodeBase> &t)> decodeByBinary,
                     std::function<void(BinaryWriter &binaryWriter, const std::unique_ptr<NodeBase> &t)> encodeByBinary);

            static std::vector<NodeType *> NODE_TYPES;

            static bool canLoadNodeJson;

            static std::unique_ptr<NodeType> UNKNOWN;
            static std::unique_ptr<NodeType> BLOCK;
            static std::unique_ptr<NodeType> BOOLEAN;
            static std::unique_ptr<NodeType> COMMAND;
            static std::unique_ptr<NodeType> COMMAND_NAME;
            static std::unique_ptr<NodeType> FLOAT;
            static std::unique_ptr<NodeType> INTEGER;
            static std::unique_ptr<NodeType> ITEM;
            static std::unique_ptr<NodeType> LF;
            static std::unique_ptr<NodeType> NAMESPACE_ID;
            static std::unique_ptr<NodeType> NORMAL_ID;
            static std::unique_ptr<NodeType> PER_COMMAND;
            static std::unique_ptr<NodeType> POSITION;
            static std::unique_ptr<NodeType> RELATIVE_FLOAT;
            static std::unique_ptr<NodeType> REPEAT;
            static std::unique_ptr<NodeType> STRING;
            static std::unique_ptr<NodeType> TARGET_SELECTOR;
            static std::unique_ptr<NodeType> TEXT;
            static std::unique_ptr<NodeType> RANGE;
            static std::unique_ptr<NodeType> XP_INTEGER;
            static std::unique_ptr<NodeType> JSON;
            static std::unique_ptr<NodeType> JSON_OBJECT;
            static std::unique_ptr<NodeType> JSON_LIST;
            static std::unique_ptr<NodeType> JSON_STRING;
            static std::unique_ptr<NodeType> JSON_INTEGER;
            static std::unique_ptr<NodeType> JSON_FLOAT;
            static std::unique_ptr<NodeType> JSON_BOOLEAN;
            static std::unique_ptr<NodeType> JSON_NULL;

            static void init();
        };

    }// namespace Node

}// namespace CHelper

#endif//CHELPER_NODETYPE_H
