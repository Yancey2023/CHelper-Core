//
// Created by Yancey on 2023/12/10.
//

#pragma once

#ifndef CHELPER_NODETYPE_H
#define CHELPER_NODETYPE_H

#include "NodeBase.h"
#include "NodeType.h"
#include "param/NodeBlock.h"
#include "param/NodeCommand.h"
#include "param/NodeCommandName.h"
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
#include "param/NodeXpInteger.h"
#include "pch.h"
#include "template/NodeTemplateBoolean.h"
#include "template/NodeTemplateNumber.h"
#include "util/NodeAny.h"
#include "util/NodeEntry.h"
#include "util/NodeSingleSymbol.h"
#include "json/NodeJsonElement.h"
#include "json/NodeJsonEntry.h"
#include "json/NodeJsonList.h"
#include "json/NodeJsonNull.h"
#include "json/NodeJsonObject.h"
#include "json/NodeJsonString.h"

#define CHELPER_CODEC_NODE_TO_JSON(v1) \
    case NodeTypeId::v1:               \
        return details::NodeCodec<NodeTypeId::v1>::template to_json(allocator, jsonValue, t);

#define CHELPER_CODEC_NODE_FROM_JSON(v1) \
    case NodeTypeId::v1:                 \
        return details::NodeCodec<NodeTypeId::v1>::template from_json(jsonValue, t);

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

        static NodeCreateStage::NodeCreateStage currentCreateStage;

        namespace details {

            template<NodeTypeId::NodeTypeId nodeTypeId>
            struct NodeTypeDetail {
                static inline const std::u16string name = u"UNKNOWN";
            };

            struct CommandParamNodeTypeDetail {
                static constexpr std::array<NodeCreateStage::NodeCreateStage, 3> nodeCreateStage = {
                        NodeCreateStage::JSON_NODE,
                        NodeCreateStage::REPEAT_NODE,
                        NodeCreateStage::COMMAND_PARAM_NODE};
                static constexpr bool isMustAfterWhiteSpace = true;
            };

            struct JsonNodeTypeDetail {
                static constexpr std::array<NodeCreateStage::NodeCreateStage, 1> nodeCreateStage = {
                        NodeCreateStage::JSON_NODE};
                static constexpr bool isMustAfterWhiteSpace = false;
            };

            struct NoneNodeTypeDetail {
                static constexpr std::array<NodeCreateStage::NodeCreateStage, 0> nodeCreateStage = {};
                static constexpr bool isMustAfterWhiteSpace = false;
            };

            template<>
            struct NodeTypeDetail<NodeTypeId::BLOCK> : CommandParamNodeTypeDetail {
                using Type = NodeBlock;
                static inline const std::u16string name = u"BLOCK";
            };

            template<>
            struct NodeTypeDetail<NodeTypeId::BOOLEAN> : CommandParamNodeTypeDetail {
                using Type = NodeBoolean;
                static inline const std::u16string name = u"BOOLEAN";
            };

            template<>
            struct NodeTypeDetail<NodeTypeId::COMMAND> : CommandParamNodeTypeDetail {
                using Type = NodeCommand;
                static inline const std::u16string name = u"COMMAND";
            };

            template<>
            struct NodeTypeDetail<NodeTypeId::COMMAND_NAME> : CommandParamNodeTypeDetail {
                using Type = NodeCommandName;
                static inline const std::u16string name = u"COMMAND_NAME";
            };

            template<>
            struct NodeTypeDetail<NodeTypeId::FLOAT> : CommandParamNodeTypeDetail {
                using Type = NodeFloat;
                static inline const std::u16string name = u"FLOAT";
            };

            template<>
            struct NodeTypeDetail<NodeTypeId::INTEGER> : CommandParamNodeTypeDetail {
                using Type = NodeInteger;
                static inline const std::u16string name = u"INTEGER";
            };

            template<>
            struct NodeTypeDetail<NodeTypeId::ITEM> : CommandParamNodeTypeDetail {
                using Type = NodeItem;
                static inline const std::u16string name = u"ITEM";
            };

            template<>
            struct NodeTypeDetail<NodeTypeId::LF> : NoneNodeTypeDetail {
                using Type = NodeLF;
                static inline const std::u16string name = u"LF";
            };

            template<>
            struct NodeTypeDetail<NodeTypeId::NAMESPACE_ID> : CommandParamNodeTypeDetail {
                using Type = NodeNamespaceId;
                static inline const std::u16string name = u"NAMESPACE_ID";
            };

            template<>
            struct NodeTypeDetail<NodeTypeId::NORMAL_ID> : CommandParamNodeTypeDetail {
                using Type = NodeNormalId;
                static inline const std::u16string name = u"NORMAL_ID";
            };

            template<>
            struct NodeTypeDetail<NodeTypeId::PER_COMMAND> : CommandParamNodeTypeDetail {
                using Type = NodePerCommand;
                static inline const std::u16string name = u"PER_COMMAND";
            };

            template<>
            struct NodeTypeDetail<NodeTypeId::POSITION> : CommandParamNodeTypeDetail {
                using Type = NodePosition;
                static inline const std::u16string name = u"POSITION";
                static constexpr bool isMustAfterWhiteSpace = false;
            };

            template<>
            struct NodeTypeDetail<NodeTypeId::RELATIVE_FLOAT> : CommandParamNodeTypeDetail {
                using Type = NodeRelativeFloat;
                static inline const std::u16string name = u"RELATIVE_FLOAT";
                static constexpr bool isMustAfterWhiteSpace = false;
            };

            template<>
            struct NodeTypeDetail<NodeTypeId::REPEAT> : CommandParamNodeTypeDetail {
                using Type = NodeRepeat;
                static inline const std::u16string name = u"REPEAT";
                static constexpr bool isMustAfterWhiteSpace = false;
            };

            template<>
            struct NodeTypeDetail<NodeTypeId::STRING> : CommandParamNodeTypeDetail {
                using Type = NodeString;
                static inline const std::u16string name = u"STRING";
            };

            template<>
            struct NodeTypeDetail<NodeTypeId::TARGET_SELECTOR> : CommandParamNodeTypeDetail {
                using Type = NodeTargetSelector;
                static inline const std::u16string name = u"TARGET_SELECTOR";
            };

            template<>
            struct NodeTypeDetail<NodeTypeId::TEXT> : CommandParamNodeTypeDetail {
                using Type = NodeText;
                static inline const std::u16string name = u"TEXT";
            };

            template<>
            struct NodeTypeDetail<NodeTypeId::RANGE> : CommandParamNodeTypeDetail {
                using Type = NodeRange;
                static inline const std::u16string name = u"RANGE";
            };

            template<>
            struct NodeTypeDetail<NodeTypeId::XP_INTEGER> : CommandParamNodeTypeDetail {
                using Type = NodeXpInteger;
                static inline const std::u16string name = u"XP_INTEGER";
            };

            template<>
            struct NodeTypeDetail<NodeTypeId::JSON> : CommandParamNodeTypeDetail {
                using Type = NodeJson;
                static inline const std::u16string name = u"JSON";
            };

            template<>
            struct NodeTypeDetail<NodeTypeId::JSON_BOOLEAN> : JsonNodeTypeDetail {
                using Type = NodeJsonBoolean;
                static inline const std::u16string name = u"JSON_BOOLEAN";
            };

            template<>
            struct NodeTypeDetail<NodeTypeId::JSON_ELEMENT> : JsonNodeTypeDetail {
                using Type = NodeJsonElement;
                static inline const std::u16string name = u"JSON_ELEMENT";
            };

            template<>
            struct NodeTypeDetail<NodeTypeId::JSON_ENTRY> : JsonNodeTypeDetail {
                using Type = NodeJsonEntry;
                static inline const std::u16string name = u"JSON_ENTRY";
            };

            template<>
            struct NodeTypeDetail<NodeTypeId::JSON_FLOAT> : JsonNodeTypeDetail {
                using Type = NodeJsonFloat;
                static inline const std::u16string name = u"JSON_FLOAT";
            };

            template<>
            struct NodeTypeDetail<NodeTypeId::JSON_INTEGER> : JsonNodeTypeDetail {
                using Type = NodeJsonInteger;
                static inline const std::u16string name = u"JSON_INTEGER";
            };

            template<>
            struct NodeTypeDetail<NodeTypeId::JSON_LIST> : JsonNodeTypeDetail {
                using Type = NodeJsonList;
                static inline const std::u16string name = u"JSON_LIST";
            };

            template<>
            struct NodeTypeDetail<NodeTypeId::JSON_NULL> : JsonNodeTypeDetail {
                using Type = NodeJsonNull;
                static inline const std::u16string name = u"JSON_NULL";
            };

            template<>
            struct NodeTypeDetail<NodeTypeId::JSON_OBJECT> : JsonNodeTypeDetail {
                using Type = NodeJsonObject;
                static inline const std::u16string name = u"JSON_OBJECT";
            };

            template<>
            struct NodeTypeDetail<NodeTypeId::JSON_STRING> : JsonNodeTypeDetail {
                using Type = NodeJsonString;
                static inline const std::u16string name = u"JSON_STRING";
            };

            template<>
            struct NodeTypeDetail<NodeTypeId::AND> : JsonNodeTypeDetail {
                using Type = NodeAnd;
                static inline const std::u16string name = u"AND";
            };

            template<>
            struct NodeTypeDetail<NodeTypeId::ANY> : JsonNodeTypeDetail {
                using Type = NodeAny;
                static inline const std::u16string name = u"ANY";
            };

            template<>
            struct NodeTypeDetail<NodeTypeId::ENTRY> : JsonNodeTypeDetail {
                using Type = NodeEntry;
                static inline const std::u16string name = u"ENTRY";
            };

            template<>
            struct NodeTypeDetail<NodeTypeId::EQUAL_ENTRY> : JsonNodeTypeDetail {
                using Type = NodeEqualEntry;
                static inline const std::u16string name = u"EQUAL_ENTRY";
            };

            template<>
            struct NodeTypeDetail<NodeTypeId::LIST> : JsonNodeTypeDetail {
                using Type = NodeList;
                static inline const std::u16string name = u"LIST";
            };

            template<>
            struct NodeTypeDetail<NodeTypeId::OR> : JsonNodeTypeDetail {
                using Type = NodeOr;
                static inline const std::u16string name = u"OR";
            };

            template<>
            struct NodeTypeDetail<NodeTypeId::SINGLE_SYMBOL> : JsonNodeTypeDetail {
                using Type = NodeSingleSymbol;
                static inline const std::u16string name = u"SINGLE_SYMBOL";
            };

            template<NodeTypeId::NodeTypeId nodeTypeId>
            struct NodeCodec {

                using Type = typename NodeTypeDetail<nodeTypeId>::Type;
                static constexpr auto nodeCreateStage = NodeTypeDetail<nodeTypeId>::nodeCreateStage;
                static inline const auto name = NodeTypeDetail<nodeTypeId>::name;
                static constexpr auto isMustAfterWhiteSpace = NodeTypeDetail<nodeTypeId>::isMustAfterWhiteSpace;

                template<class JsonValueType>
                static void to_json(typename JsonValueType::AllocatorType &allocator,
                                    JsonValueType &jsonValue,
                                    const std::unique_ptr<NodeBase> &t) {
#ifdef CHelperDebug
                    assert(serialization::Codec<Type>::enable || nodeCreateStage.empty());
#endif
                    if constexpr (serialization::Codec<Type>::enable) {
                        serialization::Codec<Type>::template to_json(allocator, jsonValue, *reinterpret_cast<Type *>(t.get()));
                    }
                }

                template<class JsonValueType>
                static void from_json(const JsonValueType &jsonValue,
                                      std::unique_ptr<NodeBase> &t) {
#ifdef CHelperDebug
                    assert(serialization::Codec<Type>::enable || nodeCreateStage.empty());
#endif
                    if constexpr (!serialization::Codec<Type>::enable) {
                        Profile::push("unknown node type -> {}", name);
                        throw std::runtime_error("unknown node type");
                    } else {
                        if (std::find(nodeCreateStage.begin(), nodeCreateStage.end(), currentCreateStage) == nodeCreateStage.end()) {
                            Profile::push("unknown node type -> {}", name);
                            throw std::runtime_error("unknown node type");
                        }
                        t = std::make_unique<Type>();
                        serialization::Codec<Type>::template from_json(jsonValue, *reinterpret_cast<Type *>(t.get()));
                        if (HEDLEY_UNLIKELY(!t->isMustAfterWhiteSpace.has_value())) {
                            t->isMustAfterWhiteSpace = isMustAfterWhiteSpace;
                        }
                    }
                }

                template<bool isNeedConvert>
                static void to_binary(std::ostream &ostream,
                                      const std::unique_ptr<NodeBase> &t) {
#ifdef CHelperDebug
                    assert(serialization::Codec<Type>::enable || nodeCreateStage.empty());
#endif
                    if constexpr (serialization::Codec<Type>::enable) {
                        serialization::Codec<Type>::template to_binary<isNeedConvert>(ostream, *reinterpret_cast<Type *>(t.get()));
                    }
                }

                template<bool isNeedConvert>
                static void from_binary(std::istream &istream,
                                        std::unique_ptr<NodeBase> &t) {
#ifdef CHelperDebug
                    assert(serialization::Codec<Type>::enable || nodeCreateStage.empty());
#endif
                    if constexpr (!serialization::Codec<Type>::enable) {
                        Profile::push("unknown node type -> {}", name);
                        throw std::runtime_error("unknown node type");
                    } else {
                        if (std::find(nodeCreateStage.begin(), nodeCreateStage.end(), currentCreateStage) == nodeCreateStage.end()) {
                            Profile::push("unknown node type -> {}", name);
                            throw std::runtime_error("unknown node type");
                        }
                        t = std::make_unique<Type>();
                        serialization::Codec<Type>::template from_binary<isNeedConvert>(istream, *reinterpret_cast<Type *>(t.get()));
                        if (HEDLEY_UNLIKELY(!t->isMustAfterWhiteSpace.has_value())) {
                            t->isMustAfterWhiteSpace = isMustAfterWhiteSpace;
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
                                const std::unique_ptr<NodeBase> &t) {
                switch (id) {
                    CODEC_PASTE(CHELPER_CODEC_NODE_TO_JSON, CHELPER_NODE_TYPES)
                }
            }

            template<class JsonValueType>
            static void from_json(const NodeTypeId::NodeTypeId id,
                                  const JsonValueType &jsonValue,
                                  std::unique_ptr<NodeBase> &t) {
                switch (id) {
                    CODEC_PASTE(CHELPER_CODEC_NODE_FROM_JSON, CHELPER_NODE_TYPES)
                }
            }

            template<bool isNeedConvert>
            static void to_binary(const NodeTypeId::NodeTypeId id,
                                  std::ostream &ostream,
                                  const std::unique_ptr<NodeBase> &t) {
                switch (id) {
                    CODEC_PASTE(CHELPER_CODEC_NODE_TO_BINARY, CHELPER_NODE_TYPES)
                }
            }

            template<bool isNeedConvert>
            static void from_binary(const NodeTypeId::NodeTypeId id,
                                    std::istream &istream,
                                    std::unique_ptr<NodeBase> &t) {
                switch (id) {
                    CODEC_PASTE(CHELPER_CODEC_NODE_FROM_BINARY, CHELPER_NODE_TYPES)
                }
            }

            static std::u16string getName(const NodeTypeId::NodeTypeId id) {
                switch (id) {
                    CODEC_PASTE(CHELPER_CODEC_GET_NAME, CHELPER_NODE_TYPES)
                }
                return u"UNKNOWN";
            }
        };

    }// namespace Node

}// namespace CHelper

CODEC_ENUM(CHelper::Node::NodeTypeId::NodeTypeId, uint8_t);

template<class JsonValueType>
void serialization::Codec<std::unique_ptr<CHelper::Node::NodeBase>>::to_json(
        typename JsonValueType::AllocatorType &allocator,
        JsonValueType &jsonValue,
        const Type &t) {
    std::u16string nodeIdName = CHelper::Node::NodeTypeHelper::getName(t->getNodeType());
    CHelper::Node::NodeTypeHelper::template to_json(t->getNodeType(), allocator, jsonValue, t);
    assert(jsonValue.IsObject());
    Codec<decltype(nodeIdName)>::template to_json_member(allocator, jsonValue, "type", nodeIdName);
}

template<class JsonValueType>
void serialization::Codec<std::unique_ptr<CHelper::Node::NodeBase>>::from_json(
        const JsonValueType &jsonValue,
        Type &t) {
    if (HEDLEY_UNLIKELY(!jsonValue.IsObject())) {
        throw exceptions::JsonSerializationTypeException("object", getJsonTypeStr(jsonValue.GetType()));
    }
    CHelper::Profile::push("loading type");
    std::u16string type;
    Codec<decltype(type)>::template from_json_member(jsonValue, "type", type);
    std::optional<std::u16string> id;
    Codec<decltype(id)>::template from_json_member(jsonValue, "id", id);
    CHelper::Profile::next("loading node {}", type);
    if (HEDLEY_LIKELY(id.has_value())) {
        CHelper::Profile::next("loading node {} with id \"{}\"", type, id.value());
    } else {
        CHelper::Profile::next("loading node {} without id", type);
    }
    std::optional<CHelper::Node::NodeTypeId::NodeTypeId> nodeTypeId;
    for (uint8_t i = 0; i <= CHelper::Node::MAX_TYPE_ID; ++i) {
        if (CHelper::Node::NodeTypeHelper::getName(static_cast<CHelper::Node::NodeTypeId::NodeTypeId>(i)) == type) {
            nodeTypeId = static_cast<CHelper::Node::NodeTypeId::NodeTypeId>(i);
            break;
        }
    }
    if (HEDLEY_UNLIKELY(nodeTypeId == std::nullopt)) {
        CHelper::Profile::next("unknown node type -> {}", type);
        throw std::runtime_error("unknown node type");
    }
    CHelper::Node::NodeTypeHelper::template from_json(nodeTypeId.value(), jsonValue, t);
    CHelper::Profile::pop();
}

template<bool isNeedConvert>
void serialization::Codec<std::unique_ptr<CHelper::Node::NodeBase>>::to_binary(
        std::ostream &ostream,
        const Type &t) {
    Codec<decltype(t->getNodeType())>::template to_binary<isNeedConvert>(ostream, t->getNodeType());
    CHelper::Node::NodeTypeHelper::template to_binary<isNeedConvert>(t->getNodeType(), ostream, t);
}

template<bool isNeedConvert>
void serialization::Codec<std::unique_ptr<CHelper::Node::NodeBase>>::from_binary(
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
