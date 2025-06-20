//
// Created by Yancey on 2023/12/10.
//

#pragma once

#ifndef CHELPER_NODETYPE_H
#define CHELPER_NODETYPE_H

#include <chelper/node/CommandNode.h>
#include <pch.h>

namespace CHelper {

    class CPack;

    namespace Node {

        class NodeSerializable;

        namespace NodeCreateStage {
            enum NodeCreateStage : uint8_t {
                NONE,
                NODE_TYPE,
                JSON_NODE,
                REPEAT_NODE,
                COMMAND_PARAM_NODE
            };
        }// namespace NodeCreateStage

        template<NodeTypeId::NodeTypeId nodeTypeId>
        struct NodeTypeDetail {
            static constexpr auto name = "UNKNOWN";
        };

        struct CommandParamNodeTypeDetail {
            static constexpr std::array<NodeCreateStage::NodeCreateStage, 3> nodeCreateStage = {
                    NodeCreateStage::JSON_NODE,
                    NodeCreateStage::REPEAT_NODE,
                    NodeCreateStage::COMMAND_PARAM_NODE};
            static constexpr bool isMustAfterSpace = true;
        };

        struct JsonNodeTypeDetail {
            static constexpr std::array<NodeCreateStage::NodeCreateStage, 1> nodeCreateStage = {
                    NodeCreateStage::JSON_NODE};
            static constexpr bool isMustAfterSpace = false;
        };

        struct UnserializableNodeTypeDetail {
            static constexpr std::array<NodeCreateStage::NodeCreateStage, 0> nodeCreateStage = {};
            static constexpr bool isMustAfterSpace = false;
        };

        template<>
        struct NodeTypeDetail<NodeTypeId::WRAPPED> : UnserializableNodeTypeDetail {
            using Type = NodeWrapped;
            static constexpr auto name = "WRAPPED";
        };

        template<>
        struct NodeTypeDetail<NodeTypeId::BLOCK> : CommandParamNodeTypeDetail {
            using Type = NodeBlock;
            static constexpr auto name = "BLOCK";
        };

        template<>
        struct NodeTypeDetail<NodeTypeId::BOOLEAN> : CommandParamNodeTypeDetail {
            using Type = NodeBoolean;
            static constexpr auto name = "BOOLEAN";
        };

        template<>
        struct NodeTypeDetail<NodeTypeId::COMMAND> : CommandParamNodeTypeDetail {
            using Type = NodeCommand;
            static constexpr auto name = "COMMAND";
        };

        template<>
        struct NodeTypeDetail<NodeTypeId::COMMAND_NAME> : CommandParamNodeTypeDetail {
            using Type = NodeCommandName;
            static constexpr auto name = "COMMAND_NAME";
        };

        template<>
        struct NodeTypeDetail<NodeTypeId::FLOAT> : CommandParamNodeTypeDetail {
            using Type = NodeFloat;
            static constexpr auto name = "FLOAT";
        };

        template<>
        struct NodeTypeDetail<NodeTypeId::INTEGER> : CommandParamNodeTypeDetail {
            using Type = NodeInteger;
            static constexpr auto name = "INTEGER";
        };

        template<>
        struct NodeTypeDetail<NodeTypeId::INTEGER_WITH_UNIT> : CommandParamNodeTypeDetail {
            using Type = NodeIntegerWithUnit;
            static constexpr auto name = "INTEGER_WITH_UNIT";
        };

        template<>
        struct NodeTypeDetail<NodeTypeId::ITEM> : CommandParamNodeTypeDetail {
            using Type = NodeItem;
            static constexpr auto name = "ITEM";
        };

        template<>
        struct NodeTypeDetail<NodeTypeId::LF> : UnserializableNodeTypeDetail {
            using Type = NodeLF;
            static constexpr auto name = "LF";
        };

        template<>
        struct NodeTypeDetail<NodeTypeId::NAMESPACE_ID> : CommandParamNodeTypeDetail {
            using Type = NodeNamespaceId;
            static constexpr auto name = "NAMESPACE_ID";
        };

        template<>
        struct NodeTypeDetail<NodeTypeId::NORMAL_ID> : CommandParamNodeTypeDetail {
            using Type = NodeNormalId;
            static constexpr auto name = "NORMAL_ID";
        };

        template<>
        struct NodeTypeDetail<NodeTypeId::PER_COMMAND> : UnserializableNodeTypeDetail {
            using Type = NodePerCommand;
            static constexpr auto name = "PER_COMMAND";
        };

        template<>
        struct NodeTypeDetail<NodeTypeId::POSITION> : CommandParamNodeTypeDetail {
            using Type = NodePosition;
            static constexpr auto name = "POSITION";
            static constexpr bool isMustAfterSpace = false;
        };

        template<>
        struct NodeTypeDetail<NodeTypeId::RELATIVE_FLOAT> : CommandParamNodeTypeDetail {
            using Type = NodeRelativeFloat;
            static constexpr auto name = "RELATIVE_FLOAT";
            static constexpr bool isMustAfterSpace = false;
        };

        template<>
        struct NodeTypeDetail<NodeTypeId::REPEAT> : CommandParamNodeTypeDetail {
            using Type = NodeRepeat;
            static constexpr auto name = "REPEAT";
            static constexpr bool isMustAfterSpace = false;
        };

        template<>
        struct NodeTypeDetail<NodeTypeId::STRING> : CommandParamNodeTypeDetail {
            using Type = NodeString;
            static constexpr auto name = "STRING";
        };

        template<>
        struct NodeTypeDetail<NodeTypeId::TARGET_SELECTOR> : CommandParamNodeTypeDetail {
            using Type = NodeTargetSelector;
            static constexpr auto name = "TARGET_SELECTOR";
        };

        template<>
        struct NodeTypeDetail<NodeTypeId::TEXT> : CommandParamNodeTypeDetail {
            using Type = NodeText;
            static constexpr auto name = "TEXT";
        };

        template<>
        struct NodeTypeDetail<NodeTypeId::RANGE> : CommandParamNodeTypeDetail {
            using Type = NodeRange;
            static constexpr auto name = "RANGE";
        };

        template<>
        struct NodeTypeDetail<NodeTypeId::JSON> : CommandParamNodeTypeDetail {
            using Type = NodeJson;
            static constexpr auto name = "JSON";
        };

        template<>
        struct NodeTypeDetail<NodeTypeId::JSON_BOOLEAN> : JsonNodeTypeDetail {
            using Type = NodeJsonBoolean;
            static constexpr auto name = "JSON_BOOLEAN";
        };

        template<>
        struct NodeTypeDetail<NodeTypeId::JSON_ELEMENT> : UnserializableNodeTypeDetail {
            using Type = NodeJsonElement;
            static constexpr auto name = "JSON_ELEMENT";
        };

        template<>
        struct NodeTypeDetail<NodeTypeId::JSON_ENTRY> : UnserializableNodeTypeDetail {
            using Type = NodeJsonEntry;
            static constexpr auto name = "JSON_ENTRY";
        };

        template<>
        struct NodeTypeDetail<NodeTypeId::JSON_FLOAT> : JsonNodeTypeDetail {
            using Type = NodeJsonFloat;
            static constexpr auto name = "JSON_FLOAT";
        };

        template<>
        struct NodeTypeDetail<NodeTypeId::JSON_INTEGER> : JsonNodeTypeDetail {
            using Type = NodeJsonInteger;
            static constexpr auto name = "JSON_INTEGER";
        };

        template<>
        struct NodeTypeDetail<NodeTypeId::JSON_LIST> : JsonNodeTypeDetail {
            using Type = NodeJsonList;
            static constexpr auto name = "JSON_LIST";
        };

        template<>
        struct NodeTypeDetail<NodeTypeId::JSON_NULL> : JsonNodeTypeDetail {
            using Type = NodeJsonNull;
            static constexpr auto name = "JSON_NULL";
        };

        template<>
        struct NodeTypeDetail<NodeTypeId::JSON_OBJECT> : JsonNodeTypeDetail {
            using Type = NodeJsonObject;
            static constexpr auto name = "JSON_OBJECT";
        };

        template<>
        struct NodeTypeDetail<NodeTypeId::JSON_STRING> : JsonNodeTypeDetail {
            using Type = NodeJsonString;
            static constexpr auto name = "JSON_STRING";
        };

        template<>
        struct NodeTypeDetail<NodeTypeId::AND> : UnserializableNodeTypeDetail {
            using Type = NodeAnd;
            static constexpr auto name = "AND";
        };

        template<>
        struct NodeTypeDetail<NodeTypeId::ANY> : UnserializableNodeTypeDetail {
            using Type = NodeAny;
            static constexpr auto name = "ANY";
        };

        template<>
        struct NodeTypeDetail<NodeTypeId::ENTRY> : UnserializableNodeTypeDetail {
            using Type = NodeEntry;
            static constexpr auto name = "ENTRY";
        };

        template<>
        struct NodeTypeDetail<NodeTypeId::EQUAL_ENTRY> : UnserializableNodeTypeDetail {
            using Type = NodeEqualEntry;
            static constexpr auto name = "EQUAL_ENTRY";
        };

        template<>
        struct NodeTypeDetail<NodeTypeId::LIST> : UnserializableNodeTypeDetail {
            using Type = NodeList;
            static constexpr auto name = "LIST";
        };

        template<>
        struct NodeTypeDetail<NodeTypeId::OR> : UnserializableNodeTypeDetail {
            using Type = NodeOr;
            static constexpr auto name = "OR";
        };

        template<>
        struct NodeTypeDetail<NodeTypeId::SINGLE_SYMBOL> : UnserializableNodeTypeDetail {
            using Type = NodeSingleSymbol;
            static constexpr auto name = "SINGLE_SYMBOL";
        };

    }// namespace Node

}// namespace CHelper

#endif//CHELPER_NODETYPE_H
