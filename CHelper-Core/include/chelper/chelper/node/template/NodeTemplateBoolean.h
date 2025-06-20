//
// Created by Yancey on 2023/11/11.
//

#pragma once

#ifndef CHELPER_NODEBOOLEAN_H
#define CHELPER_NODEBOOLEAN_H

#include "../NodeBase.h"

namespace CHelper::Node {

    template<bool isJson>
    class NodeTemplateBoolean : public NodeSerializable {
    public:
        std::optional<std::u16string> descriptionTrue, descriptionFalse;

        NodeTemplateBoolean() = default;

        [[nodiscard]] NodeTypeId::NodeTypeId getNodeType() const override {
            if constexpr (!isJson) {
                return NodeTypeId::BOOLEAN;
            } else {
                return NodeTypeId::JSON_BOOLEAN;
            }
        }

        static std::unique_ptr<NodeTemplateBoolean> make(const std::optional<std::string> &id,
                                                         const std::optional<std::u16string> &description,
                                                         const std::optional<std::u16string> &descriptionTrue,
                                                         const std::optional<std::u16string> &descriptionFalse) {
            auto node = std::make_unique<NodeTemplateBoolean>();
            node->id = id;
            node->description = description;
            node->isMustAfterSpace = false;
            node->descriptionTrue = descriptionTrue;
            node->descriptionFalse = descriptionFalse;
            return node;
        }
    };

    using NodeBoolean = NodeTemplateBoolean<false>;
    using NodeJsonBoolean = NodeTemplateBoolean<true>;

}// namespace CHelper::Node

CODEC_NODE(CHelper::Node::NodeBoolean, descriptionTrue, descriptionFalse)
CODEC_NODE(CHelper::Node::NodeJsonBoolean, descriptionTrue, descriptionFalse)

#endif//CHELPER_NODEBOOLEAN_H
