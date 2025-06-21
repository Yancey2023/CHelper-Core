//
// Created by Yancey on 2025-06-20.
//

#pragma once

#ifndef CHELPER_SERIALIZATION_H
#define CHELPER_SERIALIZATION_H

#include <chelper/node/CommandNode.h>
#include <chelper/node/NodeType.h>

#define CODEC_NODE(CodecType, ...) \
    CODEC_WITH_PARENT(CodecType, CHelper::Node::NodeSerializable, __VA_ARGS__)

#define CODEC_NODE_NONE(CodecType) \
    CODEC_NONE_WITH_PARENT(CodecType, CHelper::Node::NodeSerializable)

#define CHELPER_CODEC_NODE_TO_JSON(v1)  \
    case CHelper::Node::NodeTypeId::v1: \
        return NodeCodec<CHelper::Node::NodeTypeId::v1>::template to_json<JsonValueType>(allocator, jsonValue, t);

#define CHELPER_CODEC_NODE_FROM_JSON(v1) \
    case CHelper::Node::NodeTypeId::v1:  \
        return NodeCodec<CHelper::Node::NodeTypeId::v1>::template from_json<JsonValueType>(jsonValue, t);

#define CHELPER_CODEC_NODE_TO_BINARY(v1) \
    case CHelper::Node::NodeTypeId::v1:  \
        return NodeCodec<CHelper::Node::NodeTypeId::v1>::template to_binary<isNeedConvert>(ostream, t);

#define CHELPER_CODEC_NODE_FROM_BINARY(v1) \
    case CHelper::Node::NodeTypeId::v1:    \
        return NodeCodec<CHelper::Node::NodeTypeId::v1>::template from_binary<isNeedConvert>(istream, t);

#define CHELPER_GET_NAME(v1)            \
    case CHelper::Node::NodeTypeId::v1: \
        return CHelper::Node::NodeTypeDetail<CHelper::Node::NodeTypeId::v1>::name;

CODEC(CHelper::Node::NodeSerializable, id, brief, description, isMustAfterSpace)

template<>
struct serialization::Codec<std::unique_ptr<CHelper::Node::NodeSerializable>> : BaseCodec<std::unique_ptr<CHelper::Node::NodeSerializable>> {
    using Type = std::unique_ptr<CHelper::Node::NodeSerializable>;

    constexpr static bool enable = true;

    template<class JsonValueType>
    static void to_json(typename JsonValueType::AllocatorType &allocator,
                        JsonValueType &jsonValue,
                        const Type &t);

    template<class JsonValueType>
    static void from_json(const JsonValueType &jsonValue,
                          Type &t);

    template<bool isNeedConvert>
    static void to_binary(std::ostream &ostream,
                          const Type &t);

    template<bool isNeedConvert>
    static void from_binary(std::istream &istream,
                            Type &t);
};

CODEC_REGISTER_JSON_KEY(CHelper::Node::NodePerCommand, name, description, node, start, ast);
CODEC_REGISTER_JSON_KEY(CHelper::Node::NodeJsonElement, id, node, start);

template<>
struct serialization::Codec<CHelper::Node::NodeJsonElement> : BaseCodec<CHelper::Node::NodeJsonElement> {

    using Type = CHelper::Node::NodeJsonElement;

    constexpr static bool enable = true;

    template<class JsonValueType>
    static void to_json(typename JsonValueType::AllocatorType &allocator,
                        JsonValueType &jsonValue,
                        const Type &t) {
        jsonValue.SetObject();
        Codec<std::string>::template to_json_member<JsonValueType>(allocator, jsonValue, details::JsonKey<Type, typename JsonValueType::Ch>::id_(), t.id.value());
        Codec<decltype(t.nodes)>::template to_json_member<JsonValueType>(allocator, jsonValue, details::JsonKey<Type, typename JsonValueType::Ch>::node_(), t.nodes);
        Codec<decltype(t.startNodeId)>::template to_json_member<JsonValueType>(allocator, jsonValue, details::JsonKey<Type, typename JsonValueType::Ch>::start_(), t.startNodeId);
    }

    template<class JsonValueType>
    static void from_json(const JsonValueType &jsonValue,
                          Type &t) {
        if (HEDLEY_UNLIKELY(!jsonValue.IsObject())) {
            throw exceptions::JsonSerializationTypeException("object", getJsonTypeStr(jsonValue.GetType()));
        }
        CHelper::Profile::push("loading id");
        t.id = std::make_optional<std::string>();
        Codec<std::string>::template from_json_member<JsonValueType>(jsonValue, details::JsonKey<Type, typename JsonValueType::Ch>::id_(), t.id.value());
        CHelper::Profile::next("loading nodes");
        Codec<decltype(t.nodes)>::template from_json_member<JsonValueType>(jsonValue, details::JsonKey<Type, typename JsonValueType::Ch>::node_(), t.nodes);
        CHelper::Profile::next("loading start nodes");
        Codec<decltype(t.startNodeId)>::template from_json_member<JsonValueType>(jsonValue, details::JsonKey<Type, typename JsonValueType::Ch>::start_(), t.startNodeId);
        CHelper::Profile::pop();
    }

    template<bool isNeedConvert>
    static void to_binary(std::ostream &ostream,
                          const Type &t) {
        Codec<std::string>::template to_binary<isNeedConvert>(ostream, t.id.value());
        Codec<decltype(t.nodes)>::template to_binary<isNeedConvert>(ostream, t.nodes);
        Codec<decltype(t.startNodeId)>::template to_binary<isNeedConvert>(ostream, t.startNodeId);
    }

    template<bool isNeedConvert>
    static void from_binary(std::istream &istream,
                            Type &t) {
        t.id = std::make_optional<std::string>();
        Codec<std::string>::template from_binary<isNeedConvert>(istream, t.id.value());
        Codec<decltype(t.nodes)>::template from_binary<isNeedConvert>(istream, t.nodes);
        Codec<decltype(t.startNodeId)>::template from_binary<isNeedConvert>(istream, t.startNodeId);
    }
};// namespace serialization

template<>
struct serialization::Codec<CHelper::Node::NodePerCommand> : BaseCodec<CHelper::Node::NodePerCommand> {

    using Type = CHelper::Node::NodePerCommand;

    constexpr static bool enable = true;

    template<class JsonValueType>
    static void to_json(typename JsonValueType::AllocatorType &allocator,
                        JsonValueType &jsonValue,
                        const Type &t) {
        jsonValue.SetObject();
        //name
        Codec<decltype(t.name)>::template to_json_member<JsonValueType>(allocator, jsonValue, details::JsonKey<Type, typename JsonValueType::Ch>::name_(), t.name);
        //description
        Codec<decltype(t.description)>::template to_json_member<JsonValueType>(allocator, jsonValue, details::JsonKey<Type, typename JsonValueType::Ch>::description_(), t.description);
        //node
        Codec<decltype(t.nodes)>::template to_json_member<JsonValueType>(allocator, jsonValue, details::JsonKey<Type, typename JsonValueType::Ch>::node_(), t.nodes);
        //start
        std::vector<std::string> startIds;
        startIds.reserve(t.startNodes.size());
        for (const auto &item: t.startNodes) {
            startIds.push_back(item->innerNode->id.value());
        }
        Codec<decltype(startIds)>::template to_json_member<JsonValueType>(allocator, jsonValue, details::JsonKey<Type, typename JsonValueType::Ch>::start_(), startIds);
        //ast
        std::vector<std::vector<std::string>> ast;
        for (const auto &item: t.wrappedNodes) {
            std::vector<std::string> ast1;
            ast1.push_back(item.innerNode->id.value());
            for (const auto &item2: item.nextNodes) {
                ast1.push_back(item2->innerNode->id.value());
            }
            ast.push_back(std::move(ast1));
        }
        Codec<decltype(ast)>::template to_json_member<JsonValueType>(allocator, jsonValue, details::JsonKey<Type, typename JsonValueType::Ch>::ast_(), ast);
    }

    template<class JsonValueType>
    static void from_json(const JsonValueType &jsonValue,
                          Type &t) {
        if (HEDLEY_UNLIKELY(!jsonValue.IsObject())) {
            throw exceptions::JsonSerializationTypeException("object", getJsonTypeStr(jsonValue.GetType()));
        }
        //name
        CHelper::Profile::push("loading node name");
        Codec<decltype(t.name)>::template from_json_member<JsonValueType>(jsonValue, details::JsonKey<Type, typename JsonValueType::Ch>::name_(), t.name);
        //description
        CHelper::Profile::next("loading node description");
        Codec<decltype(t.description)>::template from_json_member<JsonValueType>(jsonValue, details::JsonKey<Type, typename JsonValueType::Ch>::description_(), t.description);
        //node
        const typename JsonValueType::ConstMemberIterator nodeIter = jsonValue.FindMember(details::JsonKey<Type, typename JsonValueType::Ch>::node_());
        if (HEDLEY_LIKELY(nodeIter != jsonValue.MemberEnd())) {
            Codec<decltype(t.nodes)>::template from_json<typename JsonValueType::ValueType>(nodeIter->value, t.nodes);
            t.wrappedNodes.reserve(t.nodes.size());
            for (auto &node: t.nodes) {
                t.wrappedNodes.emplace_back(node.get());
            }
        }
        //start
        CHelper::Profile::next("loading start nodes");
        std::vector<std::string> startNodeIds;
        Codec<decltype(startNodeIds)>::template from_json_member<JsonValueType>(jsonValue, details::JsonKey<Type, typename JsonValueType::Ch>::start_(), startNodeIds);
        t.startNodes.reserve(startNodeIds.size());
        for (const auto &startNodeId: startNodeIds) {
            CHelper::Profile::next(R"(linking startNode "{}" to nodes)", FORMAT_ARG(startNodeId));
            if (HEDLEY_UNLIKELY(startNodeId == "LF")) {
                t.startNodes.push_back(CHelper::Node::NodeLF::getInstance());
                continue;
            }
            bool flag = true;
            for (auto &node1: t.wrappedNodes) {
                if (HEDLEY_UNLIKELY(node1.innerNode->id == startNodeId)) {
                    t.startNodes.push_back(&node1);
                    flag = false;
                    break;
                }
            }
            if (HEDLEY_UNLIKELY(flag)) {
                CHelper::Profile::push(R"("unknown node id -> {} (in command \"{}\")", FORMAT_ARG(startNodeId), FORMAT_ARG(utf8::utf16to8(fmt::format(u"{}", CHelper::StringUtil::join(t.name, u",")))));
                throw std::runtime_error("unknown node id");
            }
        }
        //ast
        const typename JsonValueType::ConstMemberIterator jsonAstIter = jsonValue.FindMember(details::JsonKey<Type, typename JsonValueType::Ch>::ast_());
        if (HEDLEY_LIKELY(jsonAstIter != jsonValue.MemberEnd())) {
            CHelper::Profile::next("loading ast");
            std::vector<std::vector<std::string>> ast;
            Codec<decltype(ast)>::template from_json<typename JsonValueType::ValueType>(jsonAstIter->value, ast);
            for (const auto &childNodes: ast) {
                CHelper::Profile::next("linking child nodes to parent node");
                if (HEDLEY_UNLIKELY(childNodes.empty())) {
                    CHelper::Profile::push(R"("dismiss parent node id (in command "{}"))", FORMAT_ARG(utf8::utf16to8(fmt::format(u"{}", CHelper::StringUtil::join(t.name, u",")))));
                    throw std::runtime_error("dismiss parent node id");
                }
                auto parentNodeId = childNodes.at(0);
                CHelper::Profile::next(R"("linking child nodes to parent node "{}"))", FORMAT_ARG(parentNodeId));
                if (HEDLEY_UNLIKELY(childNodes.size() == 1)) {
                    CHelper::Profile::push(R"("dismiss parent node id, the parent node is {} (in command "{}"))", FORMAT_ARG(parentNodeId), FORMAT_ARG(utf8::utf16to8(fmt::format(u"{}", CHelper::StringUtil::join(t.name, u",")))));
                    throw std::runtime_error("dismiss parent node id");
                }
                CHelper::Node::NodeWrapped *parentNode = nullptr;
                for (auto &node1: t.wrappedNodes) {
                    if (HEDLEY_UNLIKELY(node1.innerNode->id == parentNodeId)) {
                        parentNode = &node1;
                        break;
                    }
                }
                if (HEDLEY_UNLIKELY(parentNode == nullptr)) {
                    CHelper::Profile::push(R"("unknown node id -> {} (in command "{}"))", FORMAT_ARG(parentNodeId), FORMAT_ARG(utf8::utf16to8(fmt::format(u"{}", CHelper::StringUtil::join(t.name, u",")))));
                    throw std::runtime_error("unknown node id");
                }
                if (HEDLEY_UNLIKELY(!parentNode->nextNodes.empty())) {
                    CHelper::Profile::push(R"(repeating parent node -> {} (in command "{}"))", FORMAT_ARG(parentNodeId), FORMAT_ARG(utf8::utf16to8(fmt::format(u"{}", CHelper::StringUtil::join(t.name, u",")))));
                    throw std::runtime_error("repeating parent node");
                }
                parentNode->nextNodes.reserve(childNodes.size() - 1);
                for_each(childNodes.begin() + 1, childNodes.end(), [&](const auto &childNodeId) {
                    CHelper::Profile::next(R"(linking child nodes "{}" to parent node "{} (in command "{}"))", FORMAT_ARG(childNodeId), FORMAT_ARG(parentNodeId), FORMAT_ARG(utf8::utf16to8(fmt::format(u"{}", CHelper::StringUtil::join(t.name, u",")))));
                    if (HEDLEY_UNLIKELY(childNodeId == "LF")) {
                        parentNode->nextNodes.push_back(CHelper::Node::NodeLF::getInstance());
                        return;
                    }
                    CHelper::Node::NodeWrapped *childNode = nullptr;
                    for (auto &node: t.wrappedNodes) {
                        if (HEDLEY_UNLIKELY(node.innerNode->id == childNodeId)) {
                            childNode = &node;
                            break;
                        }
                    }
                    if (HEDLEY_UNLIKELY(childNode == nullptr)) {
                        CHelper::Profile::push(R"("unknown node id -> {} (in command "{}"))", FORMAT_ARG(childNodeId), FORMAT_ARG(utf8::utf16to8(fmt::format(u"{}", CHelper::StringUtil::join(t.name, u",")))));
                        throw std::runtime_error("unknown node id");
                    }
                    parentNode->nextNodes.push_back(childNode);
                });
            }
        }
        CHelper::Profile::pop();
    }

    template<bool isNeedConvert>
    static void to_binary(std::ostream &ostream,
                          const Type &t) {
        //name
        Codec<decltype(t.name)>::template to_binary<isNeedConvert>(ostream, t.name);
        //description
        Codec<decltype(t.description)>::template to_binary<isNeedConvert>(ostream, t.description);
        //node
        Codec<decltype(t.nodes)>::template to_binary<isNeedConvert>(ostream, t.nodes);
        //start
        Codec<uint32_t>::template to_binary<isNeedConvert>(ostream, static_cast<uint32_t>(t.startNodes.size()));
        for (const auto &item: t.startNodes) {
            Codec<std::string>::template to_binary<isNeedConvert>(ostream, item->innerNode->id.value());
        }
        //ast
        for (const auto &item: t.wrappedNodes) {
            Codec<uint32_t>::template to_binary<isNeedConvert>(ostream, static_cast<uint32_t>(item.nextNodes.size()));
            for (const auto &item2: item.nextNodes) {
                Codec<std::string>::template to_binary<isNeedConvert>(ostream, item2->innerNode->id.value());
            }
        }
    }

    template<bool isNeedConvert>
    static void from_binary(std::istream &istream,
                            Type &t) {
        //name
        Codec<decltype(t.name)>::template from_binary<isNeedConvert>(istream, t.name);
        if (HEDLEY_UNLIKELY(t.name.empty())) {
            throw std::runtime_error("command size cannot be zero");
        }
        //description
        Codec<decltype(t.description)>::template from_binary<isNeedConvert>(istream, t.description);
        //node
        Codec<decltype(t.nodes)>::template from_binary<isNeedConvert>(istream, t.nodes);
        t.wrappedNodes.reserve(t.nodes.size());
        for (auto &node: t.nodes) {
            t.wrappedNodes.emplace_back(node.get());
        }
        //start
        uint32_t startNodeIdSize;
        Codec<uint32_t>::template from_binary<isNeedConvert>(istream, startNodeIdSize);
        t.startNodes.reserve(static_cast<size_t>(startNodeIdSize));
        for (size_t i = 0; i < startNodeIdSize; ++i) {
            std::string startNodeId;
            Codec<decltype(startNodeId)>::template from_binary<isNeedConvert>(istream, startNodeId);
            if (HEDLEY_UNLIKELY(startNodeId == "LF")) {
                t.startNodes.push_back(CHelper::Node::NodeLF::getInstance());
                continue;
            }
            bool flag = true;
            for (auto &node: t.wrappedNodes) {
                if (HEDLEY_UNLIKELY(node.innerNode->id == startNodeId)) {
                    t.startNodes.push_back(&node);
                    flag = false;
                    break;
                }
            }
            if (HEDLEY_UNLIKELY(flag)) {
                CHelper::Profile::push(R"("unknown node id -> {} (in command "{}"))", FORMAT_ARG(startNodeId), FORMAT_ARG(utf8::utf16to8(fmt::format(u"{}", CHelper::StringUtil::join(t.name, u",")))));
                throw std::runtime_error("unknown node id");
            }
        }
        //ast
        for (auto &parentNode: t.wrappedNodes) {
            uint32_t childNodeSize;
            Codec<uint32_t>::template from_binary<isNeedConvert>(istream, childNodeSize);
            parentNode.nextNodes.reserve(static_cast<size_t>(childNodeSize));
            for (size_t j = 0; j < childNodeSize; ++j) {
                std::string childNodeId;
                Codec<decltype(childNodeId)>::template from_binary<isNeedConvert>(istream, childNodeId);
                if (HEDLEY_UNLIKELY(childNodeId == "LF")) {
                    parentNode.nextNodes.push_back(CHelper::Node::NodeLF::getInstance());
                    continue;
                }
                CHelper::Node::NodeWrapped *childNode = nullptr;
                for (auto &node1: t.wrappedNodes) {
                    if (HEDLEY_UNLIKELY(node1.innerNode->id == childNodeId)) {
                        childNode = &node1;
                        break;
                    }
                }
                if (HEDLEY_UNLIKELY(childNode == nullptr)) {
                    CHelper::Profile::push(R"("unknown node id -> {} (in command "{}"))", FORMAT_ARG(childNodeId), FORMAT_ARG(utf8::utf16to8(fmt::format(u"{}", CHelper::StringUtil::join(t.name, u",")))));
                    throw std::runtime_error("unknown node id");
                }
                parentNode.nextNodes.push_back(childNode);
            }
        }
    }
};

static CHelper::Node::NodeCreateStage::NodeCreateStage currentCreateStage;

template<CHelper::Node::NodeTypeId::NodeTypeId nodeTypeId>
struct NodeCodec {

    using Type = typename CHelper::Node::NodeTypeDetail<nodeTypeId>::Type;
    static constexpr auto nodeCreateStage = CHelper::Node::NodeTypeDetail<nodeTypeId>::nodeCreateStage;
    static constexpr auto name = CHelper::Node::NodeTypeDetail<nodeTypeId>::name;
    static constexpr auto isMustAfterSpace = CHelper::Node::NodeTypeDetail<nodeTypeId>::isMustAfterSpace;

    template<class JsonValueType>
    static void to_json(typename JsonValueType::AllocatorType &allocator,
                        JsonValueType &jsonValue,
                        const std::unique_ptr<CHelper::Node::NodeSerializable> &t) {
        if constexpr (serialization::Codec<Type>::enable) {
            serialization::Codec<Type>::template to_json<JsonValueType>(allocator, jsonValue, *reinterpret_cast<Type *>(t.get()));
        }
    }

    template<class JsonValueType>
    static void from_json(const JsonValueType &jsonValue,
                          std::unique_ptr<CHelper::Node::NodeSerializable> &t) {
        if constexpr (!serialization::Codec<Type>::enable || nodeCreateStage.empty()) {
            CHelper::Profile::push("unknown node type -> {}", FORMAT_ARG(name));
            throw std::runtime_error("unknown node type");
        } else {
            if (std::find(nodeCreateStage.begin(), nodeCreateStage.end(), currentCreateStage) == nodeCreateStage.end()) {
                CHelper::Profile::push("unknown node type -> {}", FORMAT_ARG(name));
                throw std::runtime_error("unknown node type");
            }
            t = std::make_unique<Type>();
            t->nodeTypeId = nodeTypeId;
            serialization::Codec<Type>::template from_json<JsonValueType>(jsonValue, *reinterpret_cast<Type *>(t.get()));
            if (HEDLEY_UNLIKELY(!t->isMustAfterSpace.has_value())) {
                t->isMustAfterSpace = isMustAfterSpace;
            }
        }
    }

    template<bool isNeedConvert>
    static void to_binary(std::ostream &ostream,
                          const std::unique_ptr<CHelper::Node::NodeSerializable> &t) {
        if constexpr (serialization::Codec<Type>::enable) {
            serialization::Codec<Type>::template to_binary<isNeedConvert>(ostream, *reinterpret_cast<Type *>(t.get()));
        }
    }

    template<bool isNeedConvert>
    static void from_binary(std::istream &istream,
                            std::unique_ptr<CHelper::Node::NodeSerializable> &t) {
        if constexpr (!serialization::Codec<Type>::enable || nodeCreateStage.empty()) {
            CHelper::Profile::push("unknown node type -> {}", FORMAT_ARG(name));
            throw std::runtime_error("unknown node type");
        } else {
            if (std::find(nodeCreateStage.begin(), nodeCreateStage.end(), currentCreateStage) == nodeCreateStage.end()) {
                CHelper::Profile::push("unknown node type -> {}", FORMAT_ARG(name));
                throw std::runtime_error("unknown node type");
            }
            t = std::make_unique<Type>();
            t->nodeTypeId = nodeTypeId;
            serialization::Codec<Type>::template from_binary<isNeedConvert>(istream, *reinterpret_cast<Type *>(t.get()));
            if (HEDLEY_UNLIKELY(!t->isMustAfterSpace.has_value())) {
                t->isMustAfterSpace = isMustAfterSpace;
            }
        }
    }
};

template<CHelper::Node::NodeTypeId::NodeTypeId nodeTypeId>
struct BaseNodeUniquePtrCodec : serialization::BaseCodec<std::unique_ptr<typename CHelper::Node::NodeTypeDetail<nodeTypeId>::Type>> {

    using Type = std::unique_ptr<typename CHelper::Node::NodeTypeDetail<nodeTypeId>::Type>;

    static_assert(serialization::Codec<typename Type::element_type>::enable, "fail to find impl of Codec");

    template<class JsonValueType>
    static void to_json(typename JsonValueType::AllocatorType &allocator,
                        JsonValueType &jsonValue,
                        const Type &t) {
        serialization::Codec<typename Type::element_type>::template to_json<JsonValueType>(allocator, jsonValue, *t);
    }

    template<class JsonValueType>
    static void from_json(const JsonValueType &jsonValue,
                          Type &t) {
        t = std::make_unique<typename Type::element_type>();
        t->nodeTypeId = nodeTypeId;
        serialization::Codec<typename Type::element_type>::template from_json<JsonValueType>(jsonValue, *t);
    }

    template<bool isNeedConvert>
    static void to_binary(std::ostream &ostream,
                          const Type &t) {
        serialization::Codec<typename Type::element_type>::template to_binary<isNeedConvert>(ostream, *t);
    }

    template<bool isNeedConvert>
    static void from_binary(std::istream &istream,
                            Type &t) {
        t = std::make_unique<typename Type::element_type>();
        t->nodeTypeId = nodeTypeId;
        serialization::Codec<typename Type::element_type>::template from_binary<isNeedConvert>(istream, *t);
    }
};

template<>
struct serialization::Codec<std::unique_ptr<CHelper::Node::NodePerCommand>> : BaseNodeUniquePtrCodec<CHelper::Node::NodeTypeId::PER_COMMAND> {
    constexpr static bool enable = true;
};

template<>
struct serialization::Codec<std::unique_ptr<CHelper::Node::NodeJsonElement>> : BaseNodeUniquePtrCodec<CHelper::Node::NodeTypeId::JSON_ELEMENT> {
    constexpr static bool enable = true;
};

CODEC_NODE(CHelper::Node::NodeJsonEntry, key, value)

template<>
struct serialization::Codec<std::unique_ptr<CHelper::Node::NodeJsonEntry>> : BaseNodeUniquePtrCodec<CHelper::Node::NodeTypeId::JSON_ENTRY> {
    constexpr static bool enable = true;
};

CODEC_ENUM(CHelper::Node::NodeBlockType::NodeBlockType, uint8_t);
CODEC_NODE(CHelper::Node::NodeBlock, nodeBlockType)
CODEC_NODE_NONE(CHelper::Node::NodeCommand)
CODEC_NODE_NONE(CHelper::Node::NodeCommandName)
CODEC_NODE(CHelper::Node::NodeIntegerWithUnit, units)
CODEC_ENUM(CHelper::Node::NodeItemType::NodeItemType, uint8_t);
CODEC_NODE(CHelper::Node::NodeItem, nodeItemType)
CODEC_NODE(CHelper::Node::NodeJson, key)
CODEC_NODE(CHelper::Node::NodeNamespaceId, key, ignoreError, contents)
CODEC_NODE(CHelper::Node::NodeNormalId, key, ignoreError, contents)
CODEC_NODE_NONE(CHelper::Node::NodePosition)
CODEC_NODE_NONE(CHelper::Node::NodeRange)
CODEC_NODE(CHelper::Node::NodeRelativeFloat, canUseCaretNotation)
CODEC(CHelper::Node::RepeatData, id, breakNodes, repeatNodes, isEnd)
CODEC_NODE(CHelper::Node::NodeRepeat, key)
CODEC_NODE(CHelper::Node::NodeString, canContainSpace, ignoreLater)
CODEC_NODE(CHelper::Node::NodeTargetSelector, isMustPlayer, isMustNPC, isOnlyOne, isWildcard)
CODEC_NODE(CHelper::Node::NodeText, tokenTypes, data)
CODEC_NODE(CHelper::Node::NodeBoolean, descriptionTrue, descriptionFalse)
CODEC_NODE(CHelper::Node::NodeJsonBoolean, descriptionTrue, descriptionFalse)
CODEC_NODE(CHelper::Node::NodeFloat, min, max)
CODEC_NODE(CHelper::Node::NodeInteger, min, max)
CODEC_NODE(CHelper::Node::NodeJsonInteger, min, max)
CODEC_NODE(CHelper::Node::NodeJsonFloat, min, max)
CODEC_NODE(CHelper::Node::NodeJsonList, data)
CODEC_NODE_NONE(CHelper::Node::NodeJsonNull)
CODEC_NODE(CHelper::Node::NodeJsonObject, data)
CODEC_NODE(CHelper::Node::NodeJsonString, data)

class NodeTypeHelper {
public:
    template<class JsonValueType>
    static void to_json(const CHelper::Node::NodeTypeId::NodeTypeId id,
                        typename JsonValueType::AllocatorType &allocator,
                        JsonValueType &jsonValue,
                        const std::unique_ptr<CHelper::Node::NodeSerializable> &t) {
        switch (id) {
            CODEC_PASTE(CHELPER_CODEC_NODE_TO_JSON, CHELPER_NODE_TYPES)
            default:
                HEDLEY_UNREACHABLE();
        }
    }

    template<class JsonValueType>
    static void from_json(const CHelper::Node::NodeTypeId::NodeTypeId id,
                          const JsonValueType &jsonValue,
                          std::unique_ptr<CHelper::Node::NodeSerializable> &t) {
        switch (id) {
            CODEC_PASTE(CHELPER_CODEC_NODE_FROM_JSON, CHELPER_NODE_TYPES)
            default:
                HEDLEY_UNREACHABLE();
        }
    }

    template<bool isNeedConvert>
    static void to_binary(const CHelper::Node::NodeTypeId::NodeTypeId id,
                          std::ostream &ostream,
                          const std::unique_ptr<CHelper::Node::NodeSerializable> &t) {
        switch (id) {
            CODEC_PASTE(CHELPER_CODEC_NODE_TO_BINARY, CHELPER_NODE_TYPES)
            default:
                HEDLEY_UNREACHABLE();
        }
    }

    template<bool isNeedConvert>
    static void from_binary(const CHelper::Node::NodeTypeId::NodeTypeId id,
                            std::istream &istream,
                            std::unique_ptr<CHelper::Node::NodeSerializable> &t) {
        switch (id) {
            CODEC_PASTE(CHELPER_CODEC_NODE_FROM_BINARY, CHELPER_NODE_TYPES)
            default:
                HEDLEY_UNREACHABLE();
        }
    }

    static const char *getName(const CHelper::Node::NodeTypeId::NodeTypeId id) {
        switch (id) {
            CODEC_PASTE(CHELPER_GET_NAME, CHELPER_NODE_TYPES)
            default:
                return "UNKNOWN";
        }
    }
};

CODEC_ENUM(CHelper::Node::NodeTypeId::NodeTypeId, uint8_t);

template<class JsonValueType>
void serialization::Codec<std::unique_ptr<CHelper::Node::NodeSerializable>>::to_json(
        typename JsonValueType::AllocatorType &allocator,
        JsonValueType &jsonValue,
        const Type &t) {
    std::string nodeIdName = NodeTypeHelper::getName(t->nodeTypeId);
    NodeTypeHelper::template to_json<JsonValueType>(t->nodeTypeId, allocator, jsonValue, t);
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
        if (NodeTypeHelper::getName(static_cast<CHelper::Node::NodeTypeId::NodeTypeId>(i)) == type) {
            nodeTypeId = static_cast<CHelper::Node::NodeTypeId::NodeTypeId>(i);
            break;
        }
    }
    if (HEDLEY_UNLIKELY(nodeTypeId == std::nullopt)) {
        CHelper::Profile::next("unknown node type -> {}", FORMAT_ARG(type));
        throw std::runtime_error("unknown node type");
    }
    NodeTypeHelper::template from_json<JsonValueType>(nodeTypeId.value(), jsonValue, t);
    CHelper::Profile::pop();
}

template<bool isNeedConvert>
void serialization::Codec<std::unique_ptr<CHelper::Node::NodeSerializable>>::to_binary(
        std::ostream &ostream,
        const Type &t) {
    Codec<decltype(t->nodeTypeId)>::template to_binary<isNeedConvert>(ostream, t->nodeTypeId);
    NodeTypeHelper::template to_binary<isNeedConvert>(t->nodeTypeId, ostream, t);
}

template<bool isNeedConvert>
void serialization::Codec<std::unique_ptr<CHelper::Node::NodeSerializable>>::from_binary(
        std::istream &istream,
        Type &t) {
    CHelper::Node::NodeTypeId::NodeTypeId nodeTypeId;
    Codec<decltype(nodeTypeId)>::template from_binary<isNeedConvert>(istream, nodeTypeId);
    if (HEDLEY_UNLIKELY(nodeTypeId > CHelper::Node::MAX_TYPE_ID)) {
        throw std::runtime_error("unknown node type");
    }
    NodeTypeHelper::template from_binary<isNeedConvert>(nodeTypeId, istream, t);
}

#endif//CHELPER_SERIALIZATION_H
