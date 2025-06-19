//
// Created by Yancey on 2023/12/10.
//

#pragma once

#ifndef CHELPER_NODETYPE_H
#define CHELPER_NODETYPE_H

#include "NodeBase.h"
#include "param/NodeBlock.h"
#include "param/NodeCommand.h"
#include "param/NodeCommandName.h"
#include "param/NodeIntegerWithUnit.h"
#include "param/NodeItem.h"
#include "param/NodeJson.h"
#include "param/NodeLF.h"
#include "param/NodeNormalId.h"
#include "param/NodePosition.h"
#include "param/NodeRange.h"
#include "param/NodeRelativeFloat.h"
#include "param/NodeRepeat.h"
#include "param/NodeString.h"
#include "param/NodeTargetSelector.h"
#include "param/NodeText.h"
#include "pch.h"
#include "template/NodeTemplateBoolean.h"
#include "template/NodeTemplateNumber.h"
#include "util/NodeAny.h"
#include "util/NodeEntry.h"
#include "util/NodeSingleSymbol.h"
#include "util/NodeWrapped.h"
#include "json/NodeJsonElement.h"
#include "json/NodeJsonEntry.h"
#include "json/NodeJsonList.h"
#include "json/NodeJsonNull.h"
#include "json/NodeJsonObject.h"
#include "json/NodeJsonString.h"

#define CHELPER_CODEC_NODE_TO_JSON(v1) \
    case NodeTypeId::v1:               \
        return details::NodeCodec<NodeTypeId::v1>::template to_json<JsonValueType>(allocator, jsonValue, t);

#define CHELPER_CODEC_NODE_FROM_JSON(v1) \
    case NodeTypeId::v1:                 \
        return details::NodeCodec<NodeTypeId::v1>::template from_json<JsonValueType>(jsonValue, t);

#define CHELPER_CODEC_NODE_TO_BINARY(v1) \
    case NodeTypeId::v1:                 \
        return details::NodeCodec<NodeTypeId::v1>::template to_binary<isNeedConvert>(ostream, t);

#define CHELPER_CODEC_NODE_FROM_BINARY(v1) \
    case NodeTypeId::v1:                   \
        return details::NodeCodec<NodeTypeId::v1>::template from_binary<isNeedConvert>(istream, t);

#define CHELPER_CODEC_GET_NAME(v1) \
    case NodeTypeId::v1:           \
        return details::NodeTypeDetail<NodeTypeId::v1>::name;

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

        static NodeCreateStage::NodeCreateStage currentCreateStage;

        namespace details {

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
            struct NodeTypeDetail<NodeTypeId::SERIALIZABLE> : UnserializableNodeTypeDetail {
                using Type = NodeSerializable;
                static constexpr auto name = "SERIALIZABLE";
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

            template<NodeTypeId::NodeTypeId nodeTypeId>
            struct NodeCodec {

                using Type = typename NodeTypeDetail<nodeTypeId>::Type;
                static constexpr auto nodeCreateStage = NodeTypeDetail<nodeTypeId>::nodeCreateStage;
                static constexpr auto name = NodeTypeDetail<nodeTypeId>::name;
                static constexpr auto isMustAfterSpace = NodeTypeDetail<nodeTypeId>::isMustAfterSpace;

                template<class JsonValueType>
                static void to_json(typename JsonValueType::AllocatorType &allocator,
                                    JsonValueType &jsonValue,
                                    const std::unique_ptr<NodeSerializable> &t) {
                    if constexpr (serialization::Codec<Type>::enable) {
                        serialization::Codec<Type>::template to_json<JsonValueType>(allocator, jsonValue, *reinterpret_cast<Type *>(t.get()));
                    }
                }

                template<class JsonValueType>
                static void from_json(const JsonValueType &jsonValue,
                                      std::unique_ptr<NodeSerializable> &t) {
                    if constexpr (!serialization::Codec<Type>::enable || nodeCreateStage.empty()) {
                        Profile::push("unknown node type -> {}", FORMAT_ARG(name));
                        throw std::runtime_error("unknown node type");
                    } else {
                        if (std::find(nodeCreateStage.begin(), nodeCreateStage.end(), currentCreateStage) == nodeCreateStage.end()) {
                            Profile::push("unknown node type -> {}", FORMAT_ARG(name));
                            throw std::runtime_error("unknown node type");
                        }
                        t = std::make_unique<Type>();
                        serialization::Codec<Type>::template from_json<JsonValueType>(jsonValue, *reinterpret_cast<Type *>(t.get()));
                        if (HEDLEY_UNLIKELY(!t->isMustAfterSpace.has_value())) {
                            t->isMustAfterSpace = isMustAfterSpace;
                        }
                    }
                }

                template<bool isNeedConvert>
                static void to_binary(std::ostream &ostream,
                                      const std::unique_ptr<NodeSerializable> &t) {
                    if constexpr (serialization::Codec<Type>::enable) {
                        serialization::Codec<Type>::template to_binary<isNeedConvert>(ostream, *reinterpret_cast<Type *>(t.get()));
                    }
                }

                template<bool isNeedConvert>
                static void from_binary(std::istream &istream,
                                        std::unique_ptr<NodeSerializable> &t) {
                    if constexpr (!serialization::Codec<Type>::enable || nodeCreateStage.empty()) {
                        Profile::push("unknown node type -> {}", FORMAT_ARG(name));
                        throw std::runtime_error("unknown node type");
                    } else {
                        if (std::find(nodeCreateStage.begin(), nodeCreateStage.end(), currentCreateStage) == nodeCreateStage.end()) {
                            Profile::push("unknown node type -> {}", FORMAT_ARG(name));
                            throw std::runtime_error("unknown node type");
                        }
                        t = std::make_unique<Type>();
                        serialization::Codec<Type>::template from_binary<isNeedConvert>(istream, *reinterpret_cast<Type *>(t.get()));
                        if (HEDLEY_UNLIKELY(!t->isMustAfterSpace.has_value())) {
                            t->isMustAfterSpace = isMustAfterSpace;
                        }
                    }
                }
            };

        }// namespace details

        class NodeTypeHelper {
        public:
            template<class JsonValueType>
            static void to_json(const NodeTypeId::NodeTypeId id,
                                typename JsonValueType::AllocatorType &allocator,
                                JsonValueType &jsonValue,
                                const std::unique_ptr<NodeSerializable> &t) {
                switch (id) {
                    CODEC_PASTE(CHELPER_CODEC_NODE_TO_JSON, CHELPER_NODE_TYPES)
                    default:
                        HEDLEY_UNREACHABLE();
                }
            }

            template<class JsonValueType>
            static void from_json(const NodeTypeId::NodeTypeId id,
                                  const JsonValueType &jsonValue,
                                  std::unique_ptr<NodeSerializable> &t) {
                switch (id) {
                    CODEC_PASTE(CHELPER_CODEC_NODE_FROM_JSON, CHELPER_NODE_TYPES)
                    default:
                        HEDLEY_UNREACHABLE();
                }
            }

            template<bool isNeedConvert>
            static void to_binary(const NodeTypeId::NodeTypeId id,
                                  std::ostream &ostream,
                                  const std::unique_ptr<NodeSerializable> &t) {
                switch (id) {
                    CODEC_PASTE(CHELPER_CODEC_NODE_TO_BINARY, CHELPER_NODE_TYPES)
                    default:
                        HEDLEY_UNREACHABLE();
                }
            }

            template<bool isNeedConvert>
            static void from_binary(const NodeTypeId::NodeTypeId id,
                                    std::istream &istream,
                                    std::unique_ptr<NodeSerializable> &t) {
                switch (id) {
                    CODEC_PASTE(CHELPER_CODEC_NODE_FROM_BINARY, CHELPER_NODE_TYPES)
                    default:
                        HEDLEY_UNREACHABLE();
                }
            }

            static const char *getName(const NodeTypeId::NodeTypeId id) {
                switch (id) {
                    CODEC_PASTE(CHELPER_CODEC_GET_NAME, CHELPER_NODE_TYPES)
                    default:
                        return "UNKNOWN";
                }
            }
        };

    }// namespace Node

}// namespace CHelper

CODEC_ENUM(CHelper::Node::NodeTypeId::NodeTypeId, uint8_t);

template<class JsonValueType>
void serialization::Codec<std::unique_ptr<CHelper::Node::NodeSerializable>>::to_json(
        typename JsonValueType::AllocatorType &allocator,
        JsonValueType &jsonValue,
        const Type &t) {
    std::string nodeIdName = CHelper::Node::NodeTypeHelper::getName(t->getNodeType());
    CHelper::Node::NodeTypeHelper::template to_json<JsonValueType>(t->getNodeType(), allocator, jsonValue, t);
    assert(jsonValue.IsObject());
    Codec<decltype(nodeIdName)>::template to_json_member<JsonValueType>(allocator, jsonValue, "type", nodeIdName);
}

template<class JsonValueType>
void serialization::Codec<std::unique_ptr<CHelper::Node::NodeSerializable>>::from_json(
        const JsonValueType &jsonValue,
        Type &t) {
    if (HEDLEY_UNLIKELY(!jsonValue.IsObject())) {
        throw exceptions::JsonSerializationTypeException("object", getJsonTypeStr(jsonValue.GetType()));
    }
    CHelper::Profile::push("loading type");
    std::string type;
    Codec<decltype(type)>::template from_json_member<JsonValueType>(jsonValue, "type", type);
    std::optional<std::u16string> id;
    Codec<decltype(id)>::template from_json_member<JsonValueType>(jsonValue, "id", id);
    CHelper::Profile::next("loading node {}", FORMAT_ARG(type));
    if (HEDLEY_LIKELY(id.has_value())) {
        CHelper::Profile::next("loading node {} with id \"{}\"", FORMAT_ARG(type), FORMAT_ARG(utf8::utf16to8(id.value())));
    } else {
        CHelper::Profile::next("loading node {} without id", FORMAT_ARG(type));
    }
    std::optional<CHelper::Node::NodeTypeId::NodeTypeId> nodeTypeId;
    for (uint8_t i = 0; i <= CHelper::Node::MAX_TYPE_ID; ++i) {
        if (CHelper::Node::NodeTypeHelper::getName(static_cast<CHelper::Node::NodeTypeId::NodeTypeId>(i)) == type) {
            nodeTypeId = static_cast<CHelper::Node::NodeTypeId::NodeTypeId>(i);
            break;
        }
    }
    if (HEDLEY_UNLIKELY(nodeTypeId == std::nullopt)) {
        CHelper::Profile::next("unknown node type -> {}", FORMAT_ARG(type));
        throw std::runtime_error("unknown node type");
    }
    CHelper::Node::NodeTypeHelper::template from_json<JsonValueType>(nodeTypeId.value(), jsonValue, t);
    CHelper::Profile::pop();
}

template<bool isNeedConvert>
void serialization::Codec<std::unique_ptr<CHelper::Node::NodeSerializable>>::to_binary(
        std::ostream &ostream,
        const Type &t) {
    Codec<decltype(t->getNodeType())>::template to_binary<isNeedConvert>(ostream, t->getNodeType());
    CHelper::Node::NodeTypeHelper::template to_binary<isNeedConvert>(t->getNodeType(), ostream, t);
}

template<bool isNeedConvert>
void serialization::Codec<std::unique_ptr<CHelper::Node::NodeSerializable>>::from_binary(
        std::istream &istream,
        Type &t) {
    CHelper::Node::NodeTypeId::NodeTypeId typeId;
    Codec<decltype(typeId)>::template from_binary<isNeedConvert>(istream, typeId);
    if (HEDLEY_UNLIKELY(typeId > CHelper::Node::MAX_TYPE_ID)) {
        throw std::runtime_error("unknown typeId");
    }
    CHelper::Node::NodeTypeHelper::template from_binary<isNeedConvert>(typeId, istream, t);
}

#undef CHELPER_CODEC_NODE_TO_JSON
#undef CHELPER_CODEC_NODE_FROM_JSON
#undef CHELPER_CODEC_NODE_TO_BINARY
#undef CHELPER_CODEC_NODE_FROM_BINARY

#endif//CHELPER_NODETYPE_H
