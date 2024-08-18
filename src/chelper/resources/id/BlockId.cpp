//
// Created by Yancey on 2023/11/7.
//

#include "BlockId.h"
#include "../../node/NodeBase.h"
#include "../../node/param/NodeBoolean.h"
#include "../../node/param/NodeFloat.h"
#include "../../node/param/NodeInteger.h"
#include "../../node/param/NodeString.h"
#include "../../node/param/NodeText.h"
#include "../../node/util/NodeEntry.h"
#include "../../node/util/NodeList.h"
#include "../../node/util/NodeSingleSymbol.h"

namespace CHelper {

    static std::unique_ptr<Node::NodeBase> nodeBlockStateLeftBracket = std::make_unique<Node::NodeSingleSymbol>(
            L"BLOCK_STATE_LEFT_BRACKET", L"方块状态左括号", '[');
    static std::unique_ptr<Node::NodeBase> nodeBlockStateEntryKey = std::make_unique<Node::NodeString>(
            L"BLOCK_STATE_ENTRY_KEY", L"方块状态键值对的键", false, false, false);
    static std::unique_ptr<Node::NodeBase> nodeBlockStateEntrySeparator = std::make_unique<Node::NodeSingleSymbol>(
            L"BLOCK_STATE_ENTRY_SEPARATOR", L"方块状态键值对分隔符", '=');
    static std::unique_ptr<Node::NodeBase> nodeBlockStateEntryValueBoolean = std::make_unique<Node::NodeBoolean>(
            L"BLOCK_STATE_ENTRY_VALUE_BOOLEAN", L"方块状态键值对的值（布尔值）",
            std::nullopt, std::nullopt);
    static std::unique_ptr<Node::NodeBase> nodeBlockStateEntryValueInteger = std::make_unique<Node::NodeInteger>(
            L"BLOCK_STATE_ENTRY_VALUE_INTEGER", L"方块状态键值对的值（整数）",
            std::nullopt, std::nullopt);
    static std::unique_ptr<Node::NodeBase> nodeBlockStateEntryValueFloat = std::make_unique<Node::NodeFloat>(
            L"BLOCK_STATE_ENTRY_VALUE_FLOAT", L"方块状态键值对的值（小数）",
            std::nullopt, std::nullopt);
    static std::unique_ptr<Node::NodeBase> nodeBlockStateEntryValueString = std::make_unique<Node::NodeString>(
            L"BLOCK_STATE_ENTRY_VALUE_STRING", L"方块状态键值对的值（字符串）",
            false, true, false);
    static std::unique_ptr<Node::NodeBase> nodeBlockStateEntryAllValue = std::make_unique<Node::NodeOr>(
            L"BLOCK_STATE_ENTRY_VALUE", L"方块状态键值对的值",
            std::vector<const Node::NodeBase *>{
                    nodeBlockStateEntryValueBoolean.get(), nodeBlockStateEntryValueInteger.get(),
                    nodeBlockStateEntryValueFloat.get(), nodeBlockStateEntryValueString.get()},
            false, false, true,
            L"类型不匹配，当前内容不是有效的方块状态值");
    static std::unique_ptr<Node::NodeBase> nodeBlockStateAllEntry = std::make_unique<Node::NodeEntry>(
            L"BLOCK_STATE_ENTRY", L"方块状态键值对", nodeBlockStateEntryKey.get(),
            nodeBlockStateEntrySeparator.get(), nodeBlockStateEntryAllValue.get());
    static std::unique_ptr<Node::NodeBase> nodeBlockStateSeparator = std::make_unique<Node::NodeSingleSymbol>(
            L"BLOCK_STATE_SEPARATOR", L"方块状态分隔符", ',');
    static std::unique_ptr<Node::NodeBase> nodeBlockStateRightBracket = std::make_unique<Node::NodeSingleSymbol>(
            L"BLOCK_STATE_RIGHT_BRACKET", L"方块状态右括号", ']');
    static std::unique_ptr<Node::NodeBase> nodeAllBlockState = std::make_unique<Node::NodeList>(
            L"BLOCK_STATE", L"方块状态",
            nodeBlockStateLeftBracket.get(), nodeBlockStateAllEntry.get(),
            nodeBlockStateSeparator.get(), nodeBlockStateRightBracket.get());

    std::shared_ptr<Node::NodeBase> BlockStateValue::getNode() {
        if (HEDLEY_UNLIKELY(node == nullptr)) {
            switch (type) {
                case BlockStateType::STRING:
                    node = std::make_shared<Node::NodeText>(
                            L"BLOCK_STATE_ENTRY_VALUE_STRING", L"方块状态键值对的键（字符串）",
                            NormalId::make(L'\"' + std::get<std::wstring>(value) + L'\"', description));
                    break;
                case BlockStateType::INTEGER:
                    node = std::make_shared<Node::NodeText>(
                            L"BLOCK_STATE_ENTRY_VALUE_INTEGER", L"方块状态键值对的键（整数）",
                            NormalId::make(std::to_wstring(std::get<int32_t>(value)), description),
                            [](const Node::NodeBase *node1, TokenReader &tokenReader) -> ASTNode {
                                return tokenReader.readIntegerASTNode(node1);
                            });
                    break;
                case BlockStateType::BOOLEAN:
                    node = std::make_shared<Node::NodeText>(
                            L"BLOCK_STATE_ENTRY_VALUE_BOOLEAN", L"方块状态键值对的键（布尔值）",
                            NormalId::make(std::get<bool>(value) ? L"true" : L"false", description));
                    break;
                default:
                    HEDLEY_UNREACHABLE();
            }
        }
        return node;
    }

    std::shared_ptr<Node::NodeBase> BlockState::getNode() {
        if (HEDLEY_UNLIKELY(node == nullptr)) {
            std::vector<const Node::NodeBase *> valueNodes;
            valueNodes.reserve(values.size());
            for (auto &item: values) {
                valueNodes.push_back(item.getNode().get());
            }
            //key = value
            auto nodeKey = std::make_shared<Node::NodeText>(
                    L"BLOCK_STATE_ENTRY_KEY", L"方块状态键值对的键",
                    NormalId::make(L'\"' + key + L'\"', description));
            auto nodeValue = std::make_shared<Node::NodeOr>(
                    L"BLOCK_STATE_ENTRY_VALUE", L"方块状态键值对的值",
                    std::move(valueNodes), false);
            node = std::make_shared<Node::NodeEntry>(
                    L"BLOCK_STATE_PER_ENTRY", L"方块状态单个键值对",
                    nodeKey.get(),
                    nodeBlockStateEntrySeparator.get(),
                    nodeValue.get());
            nodeChildren.push_back(std::move(nodeKey));
            nodeChildren.push_back(std::move(nodeValue));
        }
        return node;
    }

    std::shared_ptr<Node::NodeBase> BlockId::getNode() {
        if (HEDLEY_UNLIKELY(node == nullptr)) {
            std::vector<const Node::NodeBase *> blockStateEntryChildNode2;
            //已知的方块状态
            if (HEDLEY_LIKELY(blockStates.has_value())) {
                blockStateEntryChildNode2.reserve(2);
                std::vector<const Node::NodeBase *> blockStateEntryChildNode1;
                blockStateEntryChildNode1.reserve(blockStates->size());
                std::transform(blockStates->begin(), blockStates->end(),
                               std::back_inserter(blockStateEntryChildNode1),
                               [](auto &item) -> const Node::NodeBase * {
                                   return item.getNode().get();
                               });
                auto nodeChild = std::make_shared<Node::NodeOr>(
                        L"BLOCK_STATE_ENTRY", L"方块状态键值对",
                        std::move(blockStateEntryChildNode1), false);
                blockStateEntryChildNode2.push_back(nodeChild.get());
                nodeChildren.push_back(std::move(nodeChild));
            }
            //其他未知的方块状态
            blockStateEntryChildNode2.push_back(nodeBlockStateAllEntry.get());
            //把所有方块状态拼在一起
            auto nodeValue = std::make_shared<Node::NodeOr>(L"BLOCK_STATE_ENTRY", L"方块状态键值对",
                                                            std::move(blockStateEntryChildNode2), false, true);
            node = std::make_shared<Node::NodeList>(
                    L"BLOCK_STATE",
                    L"方块状态",
                    nodeBlockStateLeftBracket.get(),
                    nodeValue.get(),
                    nodeBlockStateSeparator.get(),
                    nodeBlockStateRightBracket.get());
            nodeChildren.push_back(std::move(nodeValue));
        }
        return node;
    }

    Node::NodeBase *BlockId::getNodeAllBlockState() {
        return nodeAllBlockState.get();
    }

#if CHelperOnlyReadBinary != true
    void from_json(const nlohmann::json &j, BlockStateValue &t) {
        const nlohmann::json &jsonValue = j.at("value");
        if (HEDLEY_UNLIKELY(jsonValue.is_number_integer())) {
            t.type = BlockStateType::INTEGER;
            t.value = jsonValue.get<int32_t>();
        } else if (HEDLEY_UNLIKELY(jsonValue.is_boolean())) {
            t.type = BlockStateType::BOOLEAN;
            t.value = jsonValue.get<bool>();
        } else {
            t.type = BlockStateType::STRING;
            t.value = string2wstring(jsonValue.get<std::string>());
        }
        JsonUtil::decode<std::wstring>(j, "description", t.description);
    }

    void to_json(nlohmann::json &j, const BlockStateValue &t) {
        JsonUtil::encode(j, "description", t.description);
        switch (t.type) {
            case BlockStateType::STRING:
                JsonUtil::encode(j, "value", std::get<std::wstring>(t.value));
                break;
            case BlockStateType::BOOLEAN:
                JsonUtil::encode(j, "value", std::get<bool>(t.value));
                break;
            case BlockStateType::INTEGER:
                JsonUtil::encode(j, "value", std::get<int32_t>(t.value));
                break;
        }
    }
#endif

    void from_binary(BinaryReader &binaryReader, BlockStateValue &t) {
        binaryReader.decode(t.description);
        binaryReader.decode((uint8_t &) t.type);
        switch (t.type) {
            case BlockStateType::STRING:
                t.value = binaryReader.read<std::wstring>();
                break;
            case BlockStateType::BOOLEAN:
                t.value = binaryReader.read<bool>();
                break;
            case BlockStateType::INTEGER:
                t.value = binaryReader.read<int32_t>();
                break;
        }
    }

#if CHelperOnlyReadBinary != true
    void to_binary(BinaryWriter &binaryWriter, const BlockStateValue &t) {
        binaryWriter.encode(t.description);
        binaryWriter.encode((uint8_t) t.type);
        switch (t.type) {
            case BlockStateType::STRING:
                binaryWriter.encode(std::get<std::wstring>(t.value));
                break;
            case BlockStateType::BOOLEAN:
                binaryWriter.encode(std::get<bool>(t.value));
                break;
            case BlockStateType::INTEGER:
                binaryWriter.encode(std::get<int32_t>(t.value));
                break;
        }
    }
#endif

    CODEC(BlockState, key, description, values, defaultValue)

    CODEC_WITH_PARENT(BlockId, NamespaceId, blockStates)

}// namespace CHelper