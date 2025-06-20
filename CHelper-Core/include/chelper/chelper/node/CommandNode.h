//
// Created by Yancey on 2023/11/10.
//

#pragma once

#ifndef CHELPER_NODEBASE_H
#define CHELPER_NODEBASE_H

#include <chelper/lexer/TokenReader.h>
#include <chelper/resources/id/BlockId.h>
#include <chelper/resources/id/NormalId.h>
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
            NodeTypeId::NodeTypeId nodeTypeId;

            NodeBase() = default;

            explicit NodeBase(NodeTypeId::NodeTypeId nodeTypeId);
        };

        class NodeSerializable : public NodeBase {
        public:
            std::optional<std::string> id;
            std::optional<std::u16string> brief;
            std::optional<std::u16string> description;
            std::optional<bool> isMustAfterSpace;

            NodeSerializable() = default;

            NodeSerializable(NodeTypeId::NodeTypeId nodeTypeId,
                             const std::optional<std::string> &id,
                             const std::optional<std::u16string> &description,
                             bool isMustAfterSpace);

            [[nodiscard]] bool getIsMustAfterSpace() const;
        };

        class NodeWrapped : public NodeBase {
        public:
            NodeSerializable *innerNode;
            //存储下一个节点，需要调用构造函数之后再进行添加
            std::vector<NodeWrapped *> nextNodes;

            explicit NodeWrapped(NodeSerializable *innerNode);
        };

        class NodeAnd : public NodeBase {
        public:
            std::vector<const NodeBase *> childNodes;

            NodeAnd() = default;

            explicit NodeAnd(const std::vector<const NodeBase *> &childNodes);
        };

        class NodeAny : public NodeBase {
        public:
            std::unique_ptr<NodeBase> node;

        private:
            NodeAny() = default;

        public:
            static NodeAny *getNodeAny();
        };

        class NodeEntry : public NodeBase {
        public:
            NodeBase *nodeKey = nullptr;
            NodeBase *nodeSeparator = nullptr;
            NodeBase *nodeValue = nullptr;

            NodeEntry() = default;

            NodeEntry(NodeBase *nodeKey,
                      NodeBase *nodeSeparator,
                      NodeBase *nodeValue);
        };

        struct EqualData {
            std::u16string name;
            std::optional<std::u16string> description;
            bool canUseNotEqual;
            const NodeBase *nodeValue;

            EqualData(std::u16string name,
                      const std::optional<std::u16string> &description,
                      bool canUseNotEqual,
                      const NodeBase *nodeValue);
        };

        class NodeEqualEntry : public NodeBase {
        public:
            static std::unique_ptr<NodeBase> nodeEqual;
            static std::unique_ptr<NodeBase> nodeNotEqual;
            static std::unique_ptr<NodeBase> nodeEqualOrNotEqual;
            std::vector<EqualData> equalDatas;
            std::shared_ptr<std::vector<std::shared_ptr<NormalId>>> nodeKeyContent;
            std::unique_ptr<NodeBase> nodeKey;

            NodeEqualEntry() = default;

            explicit NodeEqualEntry(std::vector<EqualData> equalDatas);
        };

        class NodeOr : public NodeBase {
        public:
            std::vector<const NodeBase *> childNodes;
            bool isAttachToEnd = false, isUseFirst = false;
            bool noSuggestion = false;
            const char16_t *defaultErrorReason = nullptr;
            ASTNodeId::ASTNodeId nodeId = ASTNodeId::NONE;

            NodeOr() = default;

            NodeOr(std::vector<const NodeBase *> childNodes,
                   bool isAttachToEnd,
                   bool isUseFirst = false,
                   bool noSuggestion = false,
                   const char16_t *defaultErrorReason = nullptr,
                   ASTNodeId::ASTNodeId nodeId = ASTNodeId::NONE);
        };

        class NodeList : public NodeBase {
        public:
            NodeBase *nodeLeft = nullptr;
            NodeBase *nodeElement = nullptr;
            NodeBase *nodeSeparator = nullptr;
            NodeBase *nodeRight = nullptr;
            NodeOr nodeElementOrRight;
            NodeOr nodeSeparatorOrRight;

            NodeList() = default;

            NodeList(NodeBase *nodeLeft,
                     NodeBase *nodeElement,
                     NodeBase *nodeSeparator,
                     NodeBase *nodeRight);
        };

        class NodeSingleSymbol : public NodeBase {
        public:
            char16_t symbol = ' ';
            std::shared_ptr<NormalId> normalId;
            bool isAddSpace = false;

            NodeSingleSymbol() = default;

            NodeSingleSymbol(char16_t symbol,
                             const std::optional<std::u16string> &description,
                             bool isAddSpace = true);
        };

        namespace NodeBlockType {
            enum NodeBlockType : uint8_t {
                // <方块ID> [方块状态]
                BLOCK_WITH_BLOCK_STATE = 0,
                // <方块ID>
                BLOCK = 1
            };
        }// namespace NodeBlockType

        class NodeBlock : public NodeSerializable {
        public:
            static std::unique_ptr<Node::NodeSingleSymbol> nodeBlockStateLeftBracket;
            NodeBlockType::NodeBlockType nodeBlockType = NodeBlockType::BLOCK_WITH_BLOCK_STATE;
            std::shared_ptr<BlockIds> blockIds = nullptr;
            std::shared_ptr<NodeBase> nodeBlockId = nullptr;

            NodeBlock() = default;
        };

        class NodePerCommand : public NodeBase {
        public:
            std::vector<std::u16string> name;
            std::optional<std::u16string> description;
            std::vector<std::unique_ptr<Node::NodeSerializable>> nodes;
            std::vector<Node::NodeWrapped> wrappedNodes;
            std::vector<Node::NodeWrapped *> startNodes;

            NodePerCommand() = default;
        };

        class NodeCommand : public NodeSerializable {
        public:
            static std::unique_ptr<Node::NodeSingleSymbol> nodeCommandStart;

            std::vector<std::unique_ptr<NodePerCommand>> *commands = nullptr;

            NodeCommand(const std::optional<std::string> &id,
                        const std::optional<std::u16string> &description,
                        std::vector<std::unique_ptr<NodePerCommand>> *commands);

            NodeCommand() = default;
        };

        class NodeCommandName : public NodeSerializable {
        public:
            std::vector<std::unique_ptr<NodePerCommand>> *commands = nullptr;
        };

        class NodeNormalId : public NodeSerializable {
        public:
            std::optional<std::string> key;
            std::optional<std::shared_ptr<std::vector<std::shared_ptr<NormalId>>>> contents;
            std::optional<bool> ignoreError;
            bool allowMissingID = false;
            std::function<ASTNode(const NodeBase *node, TokenReader &tokenReader)> getNormalIdASTNode;
            std::shared_ptr<std::vector<std::shared_ptr<NormalId>>> customContents;

            NodeNormalId() = default;

            NodeNormalId(
                    const std::optional<std::string> &id,
                    const std::optional<std::u16string> &description,
                    const std::string &key,
                    bool ignoreError,
                    bool allowMissingID = false,
                    const std::function<ASTNode(const NodeBase *node, TokenReader &tokenReader)> &getNormalIdASTNode =
                            [](const NodeBase *node, TokenReader &tokenReader) -> ASTNode {
                        return tokenReader.readStringOrNumberASTNode(node);
                    });

            NodeNormalId(
                    const std::optional<std::string> &id,
                    const std::optional<std::u16string> &description,
                    const std::shared_ptr<std::vector<std::shared_ptr<NormalId>>> &contents,
                    bool ignoreError,
                    bool allowMissingID = false,
                    const std::function<ASTNode(const NodeBase *node, TokenReader &tokenReader)> &getNormalIdASTNode =
                            [](const NodeBase *node, TokenReader &tokenReader) -> ASTNode {
                        return tokenReader.readStringOrNumberASTNode(node);
                    });
        };

        class NodeIntegerWithUnit : public NodeSerializable {
        public:
            std::shared_ptr<std::vector<std::shared_ptr<NormalId>>> units;
            std::unique_ptr<NodeNormalId> nodeUnits;
            std::unique_ptr<NodeAnd> nodeIntegerWithUnit;
            std::unique_ptr<NodeOr> nodeIntegerMaybeHaveUnit;

            NodeIntegerWithUnit() = default;
        };

        namespace NodeItemType {
            enum NodeItemType : uint8_t {
                // <物品ID> <物品数量> <附加值> [物品组件]
                ITEM_GIVE = 0,
                // <物品ID> <附加值> <物品数量>
                ITEM_CLEAR = 1
            };
        }// namespace NodeItemType

        class NodeItem : public NodeSerializable {
        public:
            static std::shared_ptr<NodeBase> nodeCount;
            static std::shared_ptr<NodeBase> nodeAllData;
            NodeItemType::NodeItemType nodeItemType = NodeItemType::ITEM_GIVE;
            std::unique_ptr<NodeBase> nodeItemId;
            std::shared_ptr<std::vector<std::shared_ptr<ItemId>>> itemIds;
            std::unique_ptr<NodeBase> nodeComponent;

            NodeItem() = default;
        };

        class NodeJson : public NodeSerializable {
        public:
            std::string key;
            const NodeBase *nodeJson = nullptr;

            NodeJson(const std::optional<std::string> &id,
                     const std::optional<std::u16string> &description,
                     std::string key);

            NodeJson() = default;
        };

        class NodeLF : public NodeSerializable {
        private:
            NodeLF() = default;

            NodeLF(const std::optional<std::string> &id,
                   const std::optional<std::u16string> &description);

        public:
            static NodeWrapped *getInstance();
        };

        class NodeNamespaceId : public NodeSerializable {
        public:
            std::optional<std::string> key;
            std::optional<std::shared_ptr<std::vector<std::shared_ptr<NamespaceId>>>> contents;
            std::optional<bool> ignoreError;
            std::shared_ptr<std::vector<std::shared_ptr<NamespaceId>>> customContents;

            NodeNamespaceId() = default;

            NodeNamespaceId(const std::optional<std::string> &id,
                            const std::optional<std::u16string> &description,
                            const std::optional<std::string> &key,
                            bool ignoreError);
        };

        class NodePosition : public NodeSerializable {
        public:
            NodePosition(const std::optional<std::string> &id,
                         const std::optional<std::u16string> &description);

            NodePosition() = default;
        };

        class NodeRange : public NodeSerializable {
        public:
            NodeRange() = default;

            NodeRange(const std::optional<std::string> &id,
                      const std::optional<std::u16string> &description);
        };

        namespace NodeRelativeFloatType {
            enum NodeRelativeFloatType : uint8_t {
                ABSOLUTE_COORDINATE,
                RELATIVE_WORLD_COORDINATE,
                LOCAL_COORDINATE,
            };
        }// namespace NodeRelativeFloatType

        class NodeRelativeFloat : public NodeSerializable {
        public:
            static std::unique_ptr<NodeSingleSymbol> nodeRelativeNotation;
            static std::unique_ptr<NodeSingleSymbol> nodeCaretNotation;
            static std::unique_ptr<NodeOr> nodePreSymbol;

            bool canUseCaretNotation = true;

            NodeRelativeFloat() = default;

            NodeRelativeFloat(const std::optional<std::string> &id,
                              const std::optional<std::u16string> &description,
                              bool canUseCaretNotation);
        };

        struct RepeatData {
            std::string id;
            std::vector<std::unique_ptr<Node::NodeSerializable>> breakNodes;
            std::vector<std::vector<std::unique_ptr<Node::NodeSerializable>>> repeatNodes;
            std::vector<bool> isEnd;
        };

        class NodeRepeat : public NodeSerializable {
        public:
            std::string key;
            const RepeatData *repeatData = nullptr;
            const NodeBase *nodeElement = nullptr;

            NodeRepeat() = default;
        };

        class NodeString : public NodeSerializable {
        public:
            bool allowMissingString = false;
            bool canContainSpace = false;
            bool ignoreLater = false;

            NodeString() = default;

            NodeString(const std::optional<std::string> &id,
                       const std::optional<std::u16string> &description,
                       bool allowMissingString,
                       bool canContainSpace,
                       bool ignoreLater);
        };

        class NodeTargetSelector : public NodeSerializable {
        public:
            static std::shared_ptr<NodeBase> nodePlayerName;
            static std::shared_ptr<NodeBase> nodeWildcard;
            static std::shared_ptr<NodeBase> nodeAt;
            static std::shared_ptr<NodeBase> nodeTargetSelectorVariable;
            static std::shared_ptr<NodeBase> nodeLeft;
            bool isMustPlayer = false, isMustNPC = false, isOnlyOne = false, isWildcard = false;
            std::unique_ptr<NodeNamespaceId> nodeItem;
            std::unique_ptr<NodeNormalId> nodeFamily, nodeGameMode, nodeSlot;
            std::unique_ptr<NodeNamespaceId> nodeEntities;
            std::unique_ptr<NodeEqualEntry> nodeHasItemElement;
            std::unique_ptr<NodeList> nodeHasItemList1, nodeHasItemList2;
            std::unique_ptr<NodeOr> nodeHasItem;
            std::unique_ptr<NodeEqualEntry> nodeArgument;
            std::unique_ptr<NodeList> nodeArguments;

            NodeTargetSelector() = default;
        };

        class NodeText : public NodeSerializable {
        public:
            std::optional<std::vector<TokenType::TokenType>> tokenTypes;
            std::shared_ptr<NormalId> data;
            std::function<ASTNode(const NodeBase *node, TokenReader &tokenReader)> getTextASTNode;

            NodeText() = default;

            NodeText(
                    const std::optional<std::string> &id,
                    const std::optional<std::u16string> &description,
                    const std::shared_ptr<NormalId> &data,
                    const std::function<ASTNode(const NodeBase *node, TokenReader &tokenReader)> &getTextASTNode =
                            [](const NodeBase *node, TokenReader &tokenReader) -> ASTNode {
                        return tokenReader.readStringASTNode(node);
                    });
        };

        template<bool isJson>
        class NodeTemplateBoolean : public NodeSerializable {
        public:
            std::optional<std::u16string> descriptionTrue, descriptionFalse;

            NodeTemplateBoolean() = default;

            static std::unique_ptr<NodeTemplateBoolean> make(const std::optional<std::string> &id,
                                                             const std::optional<std::u16string> &description,
                                                             const std::optional<std::u16string> &descriptionTrue,
                                                             const std::optional<std::u16string> &descriptionFalse) {
                auto node = std::make_unique<NodeTemplateBoolean>();
                node->nodeTypeId = isJson ? NodeTypeId::JSON_BOOLEAN : NodeTypeId::BOOLEAN;
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

        template<class T, bool isJson>
        class NodeTemplateNumber : public NodeSerializable {
        public:
            std::optional<T> min, max;

            NodeTemplateNumber() = default;

            static auto str2number(const std::string &str, char *&end) {
                if constexpr (std::numeric_limits<T>::is_integer) {
                    return std::strtoimax(str.c_str(), &end, 10);
                } else if constexpr (std::is_same<T, float>()) {
                    return std::strtof(str.c_str(), &end);
                } else if constexpr (std::is_same<T, double>()) {
                    return std::strtod(str.c_str(), &end);
                } else if constexpr (std::is_same<T, long double>()) {
                    return std::strtold(str.c_str(), &end);
                }
            }

            static std::unique_ptr<NodeTemplateNumber<T, isJson>> make(const std::optional<std::string> &id,
                                                                       const std::optional<std::u16string> &description,
                                                                       const std::optional<T> &min0,
                                                                       const std::optional<T> &max0) {
                auto result = std::make_unique<NodeTemplateNumber<T, isJson>>();
                if constexpr (std::is_same<T, int32_t>() && !isJson) {
                    result->nodeTypeId = NodeTypeId::INTEGER;
                } else if constexpr (std::is_same<T, float>() && !isJson) {
                    result->nodeTypeId = NodeTypeId::FLOAT;
                } else if constexpr (std::is_same<T, int32_t>() && isJson) {
                    result->nodeTypeId = NodeTypeId::JSON_INTEGER;
                } else if constexpr (std::is_same<T, float>() && isJson) {
                    result->nodeTypeId = NodeTypeId::JSON_FLOAT;
                }
                result->id = id;
                result->description = description;
                result->isMustAfterSpace = false;
                result->min = min0;
                result->max = max0;
                return std::move(result);
            }
        };

        using NodeFloat = NodeTemplateNumber<float, false>;
        using NodeInteger = NodeTemplateNumber<int32_t, false>;
        using NodeJsonFloat = NodeTemplateNumber<float, true>;
        using NodeJsonInteger = NodeTemplateNumber<int32_t, true>;

        class NodeJsonElement : public NodeSerializable {
        public:
            std::vector<std::unique_ptr<NodeSerializable>> nodes;
            std::string startNodeId;
            NodeBase *start = nullptr;

            NodeJsonElement() = default;

            static NodeBase *getNodeJsonElement();
        };

        class NodeJsonEntry : public NodeSerializable {
        public:
            static std::unique_ptr<NodeBase> nodeAllEntry;
            std::u16string key;
            std::vector<std::string> value;
            std::unique_ptr<NodeBase> nodeKey, nodeValue, nodeEntry;

            NodeJsonEntry() = default;

            NodeJsonEntry(const std::optional<std::string> &id,
                          const std::optional<std::u16string> &description,
                          std::u16string key = std::u16string(),
                          std::vector<std::string> value = std::vector<std::string>());

            static NodeBase *getNodeJsonAllEntry();
        };

        class NodeJsonList : public NodeSerializable {
        public:
            static std::unique_ptr<NodeBase> nodeLeft;
            static std::unique_ptr<NodeBase> nodeRight;
            static std::unique_ptr<NodeBase> nodeSeparator;
            static std::unique_ptr<NodeBase> nodeAllList;
            std::string data;
            std::unique_ptr<NodeBase> nodeList;

            NodeJsonList() = default;

            NodeJsonList(const std::optional<std::string> &id,
                         const std::optional<std::u16string> &description,
                         std::string data = std::string());

            void init(const std::vector<std::unique_ptr<NodeSerializable>> &dataList);
        };

        class NodeJsonNull : public NodeSerializable {
        public:
            NodeJsonNull() = default;

            NodeJsonNull(const std::optional<std::string> &id,
                         const std::optional<std::u16string> &description);
        };

        class NodeJsonObject : public NodeSerializable {
        public:
            std::vector<std::unique_ptr<NodeJsonEntry>> data;
            std::unique_ptr<NodeOr> nodeElement1;
            std::unique_ptr<NodeOr> nodeElement2;
            std::unique_ptr<NodeList> nodeList;

            NodeJsonObject() = default;

            NodeJsonObject(const std::optional<std::string> &id,
                           const std::optional<std::u16string> &description);
        };

        class NodeJsonString : public NodeSerializable {
        public:
            std::optional<std::vector<std::unique_ptr<NodeSerializable>>> data;
            std::unique_ptr<NodeBase> nodeData;

            NodeJsonString() = default;

            NodeJsonString(const std::optional<std::string> &id,
                           const std::optional<std::u16string> &description);
        };

    }// namespace Node

}// namespace CHelper

#endif//CHELPER_NODEBASE_H
