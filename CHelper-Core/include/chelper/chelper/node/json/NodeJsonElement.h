//
// Created by Yancey on 2024/2/28.
//

#pragma once

#ifndef CHELPER_NODEJSONELEMENT_H
#define CHELPER_NODEJSONELEMENT_H

#include "../NodeBase.h"
#include "NodeJsonEntry.h"

namespace CHelper::Node {

    class NodeJsonElement : public NodeBase {
    public:
        std::vector<std::unique_ptr<NodeBase>> nodes;
        std::u16string startNodeId;

    private:
        NodeBase *start = nullptr;

    public:
        NodeJsonElement() = default;

        [[nodiscard]] NodeTypeId::NodeTypeId getNodeType() const override;

        void init(const CPack &cpack) override;

        ASTNode getASTNode(TokenReader &tokenReader, const CPack *cpack) const override;

        static NodeBase *getNodeJsonElement();
    };

}// namespace CHelper::Node

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
        Codec<std::u16string>::template to_json_member<JsonValueType>(allocator, jsonValue, details::JsonKey<Type, typename JsonValueType::Ch>::id_(), t.id.value());
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
        t.id = std::make_optional<std::u16string>();
        Codec<std::u16string>::template from_json_member<JsonValueType>(jsonValue, details::JsonKey<Type, typename JsonValueType::Ch>::id_(), t.id.value());
        CHelper::Profile::next("loading nodes");
        Codec<decltype(t.nodes)>::template from_json_member<JsonValueType>(jsonValue, details::JsonKey<Type, typename JsonValueType::Ch>::node_(), t.nodes);
        CHelper::Profile::next("loading start nodes");
        Codec<decltype(t.startNodeId)>::template from_json_member<JsonValueType>(jsonValue, details::JsonKey<Type, typename JsonValueType::Ch>::start_(), t.startNodeId);
        CHelper::Profile::pop();
    }

    template<bool isNeedConvert>
    static void to_binary(std::ostream &ostream,
                          const Type &t) {
        Codec<std::u16string>::template to_binary<isNeedConvert>(ostream, t.id.value());
        Codec<decltype(t.nodes)>::template to_binary<isNeedConvert>(ostream, t.nodes);
        Codec<decltype(t.startNodeId)>::template to_binary<isNeedConvert>(ostream, t.startNodeId);
    }

    template<bool isNeedConvert>
    static void from_binary(std::istream &istream,
                            Type &t) {
        t.id = std::make_optional<std::u16string>();
        Codec<std::u16string>::template from_binary<isNeedConvert>(istream, t.id.value());
        Codec<decltype(t.nodes)>::template from_binary<isNeedConvert>(istream, t.nodes);
        Codec<decltype(t.startNodeId)>::template from_binary<isNeedConvert>(istream, t.startNodeId);
    }
};// namespace serialization

CODEC_UNIQUE_PTR(CHelper::Node::NodeJsonElement)

#endif//CHELPER_NODEJSONELEMENT_H
