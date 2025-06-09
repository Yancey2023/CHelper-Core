//
// Created by Yancey on 2023/11/10.
//

#pragma once

#ifndef CHELPER_NODEBASE_H
#define CHELPER_NODEBASE_H

#include "../lexer/TokenReader.h"
#include "../parser/ASTNode.h"
#include "pch.h"

#define CHELPER_NODE_TYPES BLOCK,             \
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
                           SINGLE_SYMBOL,     \
                           WRAPPED

#define CHELPER_SERIALIZABLE_NODE_TYPES BLOCK,             \
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
                                        SINGLE_SYMBOL,     \
                                        WRAPPED

#define CODEC_NODE(CodecType, ...) \
    CODEC_WITH_PARENT(CodecType, CHelper::Node::NodeSerializable, __VA_ARGS__)

#define CODEC_NODE_NONE(CodecType) \
    CODEC_NONE_WITH_PARENT(CodecType, CHelper::Node::NodeSerializable)

namespace CHelper {

    class CPack;

    namespace Node {

        namespace NodeTypeId {
            enum NodeTypeId : uint8_t {
                CHELPER_NODE_TYPES
            };
        }

        constexpr NodeTypeId::NodeTypeId MAX_TYPE_ID = NodeTypeId::SINGLE_SYMBOL;

        class NodeBase {
        public:
            NodeBase() = default;

            virtual ~NodeBase() = default;

            virtual void init(const CPack &cpack);

            [[nodiscard]] virtual NodeTypeId::NodeTypeId getNodeType() const = 0;

            [[nodiscard]] virtual ASTNode
            getASTNode(TokenReader &tokenReader, const CPack *cpack = nullptr) const = 0;

        protected:
            HEDLEY_NON_NULL(4)
            ASTNode getByChildNode(TokenReader &tokenReader,
                                   const CPack *cpack,
                                   const NodeBase *childNode,
                                   const ASTNodeId::ASTNodeId &astNodeId = ASTNodeId::NONE) const;

        public:
            HEDLEY_NON_NULL(2)
            virtual std::optional<std::u16string> collectDescription(const ASTNode *node, size_t index) const;

            HEDLEY_NON_NULL(2)
            virtual bool collectIdError(const ASTNode *astNode,
                                        std::vector<std::shared_ptr<ErrorReason>> &idErrorReasons) const;

            HEDLEY_NON_NULL(2)
            virtual bool collectSuggestions(const ASTNode *astNode,
                                            size_t index,
                                            std::vector<Suggestions> &suggestions) const;

            virtual void collectStructure(const ASTNode *astNode,
                                          StructureBuilder &structure,
                                          bool isMustHave) const;

            HEDLEY_NON_NULL(2)
            virtual bool collectSyntax(const ASTNode *astNode,
                                       SyntaxResult &syntaxResult) const;
        };

        class NodeSerializable : public NodeBase {
        public:
            std::optional<std::string> id;
            std::optional<std::u16string> brief;
            std::optional<std::u16string> description;
            std::optional<bool> isMustAfterWhiteSpace;

            NodeSerializable() = default;

            NodeSerializable(const std::optional<std::string> &id,
                             const std::optional<std::u16string> &description,
                             bool isMustAfterWhiteSpace);

            std::optional<std::u16string> collectDescription(const ASTNode *node, size_t index) const override;

        public:
            [[nodiscard]] bool getIsMustAfterWhitespace() const;
        };

    }// namespace Node

}// namespace CHelper

CODEC(CHelper::Node::NodeSerializable, id, brief, description, isMustAfterWhiteSpace)

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

#endif//CHELPER_NODEBASE_H
