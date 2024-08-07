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

        namespace NodeCreateStage {
            enum NodeCreateStage : uint8_t {
                NONE,
                NODE_TYPE,
                JSON_NODE,
                REPEAT_NODE,
                COMMAND_PARAM_NODE
            };
        }// namespace NodeCreateStage

        class NodeType {
        public:
            uint8_t id{};
            std::string nodeName;

#if CHelperOnlyReadBinary != true
            std::function<void(const nlohmann::json &j, std::unique_ptr<NodeBase> &t)> decodeByJson;
            std::function<void(nlohmann::json &j, const std::unique_ptr<NodeBase> &t)> encodeByJson;
#endif
            std::function<void(BinaryReader &binaryReader, std::unique_ptr<NodeBase> &t)> decodeByBinary;
#if CHelperOnlyReadBinary != true
            std::function<void(BinaryWriter &binaryWriter, const std::unique_ptr<NodeBase> &t)> encodeByBinary;
#endif

#if CHelperOnlyReadBinary == true
            NodeType(std::string nodeName,
                     std::function<void(BinaryReader &binaryReader, std::unique_ptr<NodeBase> &t)> decodeByBinary);
#else
            NodeType(std::string nodeName,
                     std::function<void(const nlohmann::json &j, std::unique_ptr<NodeBase> &t)> decodeByJson,
                     std::function<void(nlohmann::json &j, const std::unique_ptr<NodeBase> &t)> encodeByJson,
                     std::function<void(BinaryReader &binaryReader, std::unique_ptr<NodeBase> &t)> decodeByBinary,
                     std::function<void(BinaryWriter &binaryWriter, const std::unique_ptr<NodeBase> &t)> encodeByBinary);
#endif

            static NodeCreateStage::NodeCreateStage currentCreateStage;

            static std::vector<NodeType *> NODE_TYPES;

            // command param node
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
            // json node
            static std::unique_ptr<NodeType> JSON_BOOLEAN;
            static std::unique_ptr<NodeType> JSON_ELEMENT;
            static std::unique_ptr<NodeType> JSON_ENTRY;
            static std::unique_ptr<NodeType> JSON_FLOAT;
            static std::unique_ptr<NodeType> JSON_INTEGER;
            static std::unique_ptr<NodeType> JSON_LIST;
            static std::unique_ptr<NodeType> JSON_NULL;
            static std::unique_ptr<NodeType> JSON_OBJECT;
            static std::unique_ptr<NodeType> JSON_STRING;
            // utility node
            static std::unique_ptr<NodeType> AND;
            static std::unique_ptr<NodeType> ANY;
            static std::unique_ptr<NodeType> ENTRY;
            static std::unique_ptr<NodeType> EQUAL_ENTRY;
            static std::unique_ptr<NodeType> LIST;
            static std::unique_ptr<NodeType> OR;
            static std::unique_ptr<NodeType> SINGLE_SYMBOL;

            static void init();
        };

    }// namespace Node

}// namespace CHelper

#endif//CHELPER_NODETYPE_H
