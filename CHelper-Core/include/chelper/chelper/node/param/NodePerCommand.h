//
// Created by Yancey on 2023/11/10.
//

#pragma once

#ifndef CHELPER_NODEPERCOMMAND_H
#define CHELPER_NODEPERCOMMAND_H

#include "../NodeBase.h"
#include "../util/NodeOr.h"
#include "NodeLF.h"

namespace CHelper::Node {

    class NodePerCommand : public NodeBase {
    public:
        std::vector<std::u16string> name;
        std::vector<std::unique_ptr<Node::NodeBase>> nodes;
        std::vector<Node::NodeBase *> startNodes;

        NodePerCommand() = default;

        void init(const CPack &cpack) override;

        [[nodiscard]] NodeTypeId::NodeTypeId getNodeType() const override;

        ASTNode getASTNode(TokenReader &tokenReader, const CPack *cpack) const override;

        std::optional<std::u16string> collectDescription(const ASTNode *node, size_t index) const override;

    };// NodePerCommand

}// namespace CHelper::Node

CODEC_REGISTER_JSON_KEY(CHelper::Node::NodePerCommand, name, description, node, start, ast);

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
        std::vector<std::u16string> startIds;
        startIds.reserve(t.startNodes.size());
        for (const auto &item: t.startNodes) {
            startIds.push_back(item->id.value());
        }
        Codec<decltype(startIds)>::template to_json_member<JsonValueType>(allocator, jsonValue, details::JsonKey<Type, typename JsonValueType::Ch>::start_(), startIds);
        //ast
        std::vector<std::vector<std::u16string>> ast;
        for (const auto &item: t.nodes) {
            std::vector<std::u16string> ast1;
            ast1.push_back(item->id.value());
            for (const auto &item2: item->nextNodes) {
                ast1.push_back(item2->id.value());
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
        }
        //start
        CHelper::Profile::next("loading start nodes");
        std::vector<std::u16string> startNodeIds;
        Codec<decltype(startNodeIds)>::template from_json_member<JsonValueType>(jsonValue, details::JsonKey<Type, typename JsonValueType::Ch>::start_(), startNodeIds);
        t.startNodes.reserve(startNodeIds.size());
        for (const auto &startNodeId: startNodeIds) {
            CHelper::Profile::next(R"(linking startNode "{}" to nodes)", FORMAT_ARG(utf8::utf16to8(startNodeId)));
            if (HEDLEY_UNLIKELY(startNodeId == u"LF")) {
                t.startNodes.push_back(CHelper::Node::NodeLF::getInstance());
                continue;
            }
            bool flag = true;
            for (auto &node1: t.nodes) {
                if (HEDLEY_UNLIKELY(node1->id == startNodeId)) {
                    t.startNodes.push_back(node1.get());
                    flag = false;
                    break;
                }
            }
            if (HEDLEY_UNLIKELY(flag)) {
                CHelper::Profile::push(R"("unknown node id -> {} (in command \"{}\")", FORMAT_ARG(utf8::utf16to8(startNodeId)), FORMAT_ARG(utf8::utf16to8(fmt::format(u"{}", CHelper::StringUtil::join(t.name, u",")))));
                throw std::runtime_error("unknown node id");
            }
        }
        //ast
        const typename JsonValueType::ConstMemberIterator jsonAstIter = jsonValue.FindMember(details::JsonKey<Type, typename JsonValueType::Ch>::ast_());
        if (HEDLEY_LIKELY(jsonAstIter != jsonValue.MemberEnd())) {
            CHelper::Profile::next("loading ast");
            std::vector<std::vector<std::u16string>> ast;
            Codec<decltype(ast)>::template from_json<typename JsonValueType::ValueType>(jsonAstIter->value, ast);
            for (const auto &childNodes: ast) {
                CHelper::Profile::next("linking child nodes to parent node");
                if (HEDLEY_UNLIKELY(childNodes.empty())) {
                    CHelper::Profile::push(R"("dismiss parent node id (in command "{}"))", FORMAT_ARG(utf8::utf16to8(fmt::format(u"{}", CHelper::StringUtil::join(t.name, u",")))));
                    throw std::runtime_error("dismiss parent node id");
                }
                auto parentNodeId = childNodes.at(0);
                CHelper::Profile::next(R"("linking child nodes to parent node "{}"))", FORMAT_ARG(utf8::utf16to8(parentNodeId)));
                if (HEDLEY_UNLIKELY(childNodes.size() == 1)) {
                    CHelper::Profile::push(R"("dismiss parent node id, the parent node is {} (in command "{}"))", FORMAT_ARG(utf8::utf16to8(parentNodeId)), FORMAT_ARG(utf8::utf16to8(fmt::format(u"{}", CHelper::StringUtil::join(t.name, u",")))));
                    throw std::runtime_error("dismiss parent node id");
                }
                CHelper::Node::NodeBase *parentNode = nullptr;
                for (auto &node1: t.nodes) {
                    if (HEDLEY_UNLIKELY(node1->id == parentNodeId)) {
                        parentNode = node1.get();
                        break;
                    }
                }
                if (HEDLEY_UNLIKELY(parentNode == nullptr)) {
                    CHelper::Profile::push(R"("unknown node id -> {} (in command "{}"))", FORMAT_ARG(utf8::utf16to8(parentNodeId)), FORMAT_ARG(utf8::utf16to8(fmt::format(u"{}", CHelper::StringUtil::join(t.name, u",")))));
                    throw std::runtime_error("unknown node id");
                }
                if (HEDLEY_UNLIKELY(!parentNode->nextNodes.empty())) {
                    CHelper::Profile::push(R"(repeating parent node -> {} (in command "{}"))", FORMAT_ARG(utf8::utf16to8(parentNodeId)), FORMAT_ARG(utf8::utf16to8(fmt::format(u"{}", CHelper::StringUtil::join(t.name, u",")))));
                    throw std::runtime_error("repeating parent node");
                }
                parentNode->nextNodes.reserve(childNodes.size() - 1);
                for_each(childNodes.begin() + 1, childNodes.end(), [&](const auto &childNodeId) {
                    CHelper::Profile::next(R"(linking child nodes "{}" to parent node "{} (in command "{}"))", FORMAT_ARG(utf8::utf16to8(childNodeId)), FORMAT_ARG(utf8::utf16to8(parentNodeId)), FORMAT_ARG(utf8::utf16to8(fmt::format(u"{}", CHelper::StringUtil::join(t.name, u",")))));
                    if (HEDLEY_UNLIKELY(childNodeId == u"LF")) {
                        parentNode->nextNodes.push_back(CHelper::Node::NodeLF::getInstance());
                        return;
                    }
                    CHelper::Node::NodeBase *childNode = nullptr;
                    for (auto &node: t.nodes) {
                        if (HEDLEY_UNLIKELY(node->id == childNodeId)) {
                            childNode = node.get();
                            break;
                        }
                    }
                    if (HEDLEY_UNLIKELY(childNode == nullptr)) {
                        CHelper::Profile::push(R"("unknown node id -> {} (in command "{}"))", FORMAT_ARG(utf8::utf16to8(childNodeId)), FORMAT_ARG(utf8::utf16to8(fmt::format(u"{}", CHelper::StringUtil::join(t.name, u",")))));
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
        Codec<uint32_t>::template to_binary<isNeedConvert>(ostream, t.startNodes.size());
        for (const auto &item: t.startNodes) {
            Codec<std::u16string>::template to_binary<isNeedConvert>(ostream, item->id.value());
        }
        //ast
        for (const auto &item: t.nodes) {
            Codec<uint32_t>::template to_binary<isNeedConvert>(ostream, item->nextNodes.size());
            for (const auto &item2: item->nextNodes) {
                Codec<std::u16string>::template to_binary<isNeedConvert>(ostream, item2->id.value());
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
        //start
        uint32_t startNodeIdSize;
        Codec<uint32_t>::template from_binary<isNeedConvert>(istream, startNodeIdSize);
        t.startNodes.reserve(startNodeIdSize);
        for (size_t i = 0; i < startNodeIdSize; ++i) {
            std::u16string startNodeId;
            Codec<decltype(startNodeId)>::template from_binary<isNeedConvert>(istream, startNodeId);
            if (HEDLEY_UNLIKELY(startNodeId == u"LF")) {
                t.startNodes.push_back(CHelper::Node::NodeLF::getInstance());
                continue;
            }
            bool flag = true;
            for (auto &node: t.nodes) {
                if (HEDLEY_UNLIKELY(node->id == startNodeId)) {
                    t.startNodes.push_back(node.get());
                    flag = false;
                    break;
                }
            }
            if (HEDLEY_UNLIKELY(flag)) {
                CHelper::Profile::push(R"("unknown node id -> {} (in command "{}"))", FORMAT_ARG(utf8::utf16to8(startNodeId)), FORMAT_ARG(utf8::utf16to8(fmt::format(u"{}", CHelper::StringUtil::join(t.name, u",")))));
                throw std::runtime_error("unknown node id");
            }
        }
        //ast
        for (const auto &node: t.nodes) {
            CHelper::Node::NodeBase *parentNode = node.get();
            uint32_t childNodeSize;
            Codec<uint32_t>::template from_binary<isNeedConvert>(istream, childNodeSize);
            parentNode->nextNodes.reserve(childNodeSize);
            for (size_t j = 0; j < childNodeSize; ++j) {
                std::u16string childNodeId;
                Codec<decltype(childNodeId)>::template from_binary<isNeedConvert>(istream, childNodeId);
                if (HEDLEY_UNLIKELY(childNodeId == u"LF")) {
                    parentNode->nextNodes.push_back(CHelper::Node::NodeLF::getInstance());
                    continue;
                }
                CHelper::Node::NodeBase *childNode = nullptr;
                for (auto &node1: t.nodes) {
                    if (HEDLEY_UNLIKELY(node1->id == childNodeId)) {
                        childNode = node1.get();
                        break;
                    }
                }
                if (HEDLEY_UNLIKELY(childNode == nullptr)) {
                    CHelper::Profile::push(R"("unknown node id -> {} (in command "{}"))", FORMAT_ARG(utf8::utf16to8(childNodeId)), FORMAT_ARG(utf8::utf16to8(fmt::format(u"{}", CHelper::StringUtil::join(t.name, u",")))));
                    throw std::runtime_error("unknown node id");
                }
                parentNode->nextNodes.push_back(childNode);
            }
        }
    }
};

CODEC_UNIQUE_PTR(CHelper::Node::NodePerCommand);

#endif//CHELPER_NODEPERCOMMAND_H
