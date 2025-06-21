//
// Created by Yancey on 2025-06-21.
//

#pragma once

#ifndef CHELPER_NODEWITHTYPE_H
#define CHELPER_NODEWITHTYPE_H

#include <pch.h>

#define CHELPER_NODE_TYPES WRAPPED,           \
                           BLOCK,             \
                           BOOLEAN,           \
                           COMMAND,           \
                           COMMAND_NAME,      \
                           FLOAT,             \
                           INTEGER,           \
                           INTEGER_WITH_UNIT, \
                           ITEM,              \
                           LF,                \
                           NAMESPACE_ID,      \
                           NORMAL_ID,         \
                           PER_COMMAND,       \
                           POSITION,          \
                           RELATIVE_FLOAT,    \
                           REPEAT,            \
                           STRING,            \
                           TARGET_SELECTOR,   \
                           TEXT,              \
                           RANGE,             \
                           JSON,              \
                           JSON_BOOLEAN,      \
                           JSON_ELEMENT,      \
                           JSON_ENTRY,        \
                           JSON_FLOAT,        \
                           JSON_INTEGER,      \
                           JSON_LIST,         \
                           JSON_NULL,         \
                           JSON_OBJECT,       \
                           JSON_STRING,       \
                           AND,               \
                           ANY,               \
                           ENTRY,             \
                           EQUAL_ENTRY,       \
                           LIST,              \
                           OR,                \
                           SINGLE_SYMBOL

namespace CHelper::Node {

    namespace NodeTypeId {
        enum NodeTypeId : uint8_t {
            CHELPER_NODE_TYPES
        };
    }// namespace NodeTypeId

    class NodeBase;

    static constexpr NodeTypeId::NodeTypeId MAX_TYPE_ID = NodeTypeId::SINGLE_SYMBOL;

    class NodeWithType {
    public:
        NodeTypeId::NodeTypeId nodeTypeId;
        Node::NodeBase *data = nullptr;

        NodeWithType() = default;

        template<class NodeType>
        NodeWithType(NodeType &node);

#ifdef CHelperDebug
        ~NodeWithType();

        NodeWithType(NodeWithType &node);

        NodeWithType(const NodeWithType &node);

        NodeWithType(NodeWithType &&node);

        NodeWithType &operator=(const NodeWithType &node);

        NodeWithType &operator=(NodeWithType &&node);
#else
        NodeWithType(NodeWithType &node) = default;
#endif
    };

    class FreeableNodeWithTypes {
    public:
        std::vector<NodeWithType> nodes;

        FreeableNodeWithTypes() = default;

        ~FreeableNodeWithTypes();

        FreeableNodeWithTypes(const FreeableNodeWithTypes &node) = delete;

        FreeableNodeWithTypes(FreeableNodeWithTypes &&node) = default;

        FreeableNodeWithTypes &operator=(const FreeableNodeWithTypes &node) = delete;

        FreeableNodeWithTypes &operator=(FreeableNodeWithTypes &&node) = default;
    };

}// namespace CHelper::Node

#endif//CHELPER_NODEWITHTYPE_H
