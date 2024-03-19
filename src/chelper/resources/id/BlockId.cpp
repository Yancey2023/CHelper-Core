//
// Created by Yancey on 2023/11/7.
//

#include "BlockId.h"
#include "../../node/NodeBase.h"
#include "../../node/param/NodeText.h"
#include "../../node/util/NodeEntry.h"
#include "../../node/util/NodeOr.h"
#include "../../node/util/NodeSingleSymbol.h"
#include "../../node/param/NodeString.h"
#include "../../node/param/NodeBoolean.h"
#include "../../node/param/NodeInteger.h"
#include "../../node/param/NodeFloat.h"
#include "../../node/util/NodeList.h"

namespace CHelper {

    static std::unique_ptr<Node::NodeBase> nodeBlockStateLeftBracket = std::make_unique<Node::NodeSingleSymbol>(
            "BLOCK_STATE_LEFT_BRACKET", "方块状态左括号", '[');
    static std::unique_ptr<Node::NodeBase> nodeBlockStateEntryKey = std::make_unique<Node::NodeString>(
            "BLOCK_STATE_ENTRY_KEY", "方块状态键值对的键", false, false, false);
    static std::unique_ptr<Node::NodeBase> nodeBlockStateEntrySeparator = std::make_unique<Node::NodeSingleSymbol>(
            "BLOCK_STATE_ENTRY_SEPARATOR", "方块状态键值对分隔符", '=');
    static std::unique_ptr<Node::NodeBase> nodeBlockStateEntryValueBoolean = std::make_unique<Node::NodeBoolean>(
            "BLOCK_STATE_ENTRY_VALUE_BOOLEAN", "方块状态键值对的值（布尔值）",
            std::nullopt, std::nullopt);
    static std::unique_ptr<Node::NodeBase> nodeBlockStateEntryValueInteger = std::make_unique<Node::NodeInteger>(
            "BLOCK_STATE_ENTRY_VALUE_INTEGER", "方块状态键值对的值（整数）",
            std::nullopt, std::nullopt);
    static std::unique_ptr<Node::NodeBase> nodeBlockStateEntryValueFloat = std::make_unique<Node::NodeFloat>(
            "BLOCK_STATE_ENTRY_VALUE_FLOAT", "方块状态键值对的值（小数）",
            std::nullopt, std::nullopt);
    static std::unique_ptr<Node::NodeBase> nodeBlockStateEntryValueString = std::make_unique<Node::NodeString>(
            "BLOCK_STATE_ENTRY_VALUE_STRING", "方块状态键值对的值（字符串）",
            false, true, false);
    static std::unique_ptr<Node::NodeBase> nodeBlockStateEntryAllValue = std::make_unique<Node::NodeOr>(
            "BLOCK_STATE_ENTRY_VALUE", "方块状态键值对的值",
            std::vector<const Node::NodeBase *>{
                    nodeBlockStateEntryValueBoolean.get(), nodeBlockStateEntryValueInteger.get(),
                    nodeBlockStateEntryValueFloat.get(), nodeBlockStateEntryValueString.get()
            }, false, false, true, "类型不匹配，当前内容不是有效的方块状态值"
    );
    static std::unique_ptr<Node::NodeBase> nodeBlockStateAllEntry = std::make_unique<Node::NodeEntry>(
            "BLOCK_STATE_ENTRY", "方块状态键值对", nodeBlockStateEntryKey.get(),
            nodeBlockStateEntrySeparator.get(), nodeBlockStateEntryAllValue.get());
    static std::unique_ptr<Node::NodeBase> nodeBlockStateSeparator = std::make_unique<Node::NodeSingleSymbol>(
            "BLOCK_STATE_SEPARATOR", "方块状态分隔符", ',');
    static std::unique_ptr<Node::NodeBase> nodeBlockStateRightBracket = std::make_unique<Node::NodeSingleSymbol>(
            "BLOCK_STATE_RIGHT_BRACKET", "方块状态右括号", ']');
    static std::unique_ptr<Node::NodeBase> nodeAllBlockState = std::make_unique<Node::NodeList>(
            "BLOCK_STATE", "方块状态",
            nodeBlockStateLeftBracket.get(), nodeBlockStateAllEntry.get(),
            nodeBlockStateSeparator.get(), nodeBlockStateRightBracket.get());

    static Node::NodeBase *getNodeValue(BlockStateType::BlockStateType type,
                                        const std::variant<std::string, int32_t, bool> &value,
                                        const std::optional<std::string> &description) {
        switch (type) {
            case BlockStateType::STRING:
                return new Node::NodeText(
                        "BLOCK_STATE_ENTRY_VALUE_STRING", "方块状态键值对的键（字符串）",
                        std::make_shared<NormalId>(std::get<std::string>(value), description));
            case BlockStateType::INTEGER:
                return new Node::NodeText(
                        "BLOCK_STATE_ENTRY_VALUE_INTEGER", "方块状态键值对的键（整数）",
                        std::make_shared<NormalId>(std::to_string(std::get<int32_t>(value)), description));
            case BlockStateType::BOOLEAN:
                return new Node::NodeText(
                        "BLOCK_STATE_ENTRY_VALUE_BOOLEAN", "方块状态键值对的键（布尔值）",
                        std::make_shared<NormalId>(std::get<bool>(value) ? "true" : "false", description));
            default:
                HEDLEY_UNREACHABLE();
        }
    }

    BlockStateValue::BlockStateValue(BlockStateType::BlockStateType type,
                                     const std::variant<std::string, int32_t, bool> &value,
                                     const std::optional<std::string> &description)
            : type(type),
              value(value),
              description(description),
              node(getNodeValue(type, value, description)) {}

    BlockStateValue::BlockStateValue(const nlohmann::json &j)
            : description(JsonUtil::fromJsonOptionalLikely<std::string>(j, "description")) {
        const nlohmann::json &jsonValue = j.at("value");
        if (HEDLEY_UNLIKELY(jsonValue.is_number_integer())) {
            type = CHelper::BlockStateType::INTEGER;
            value = jsonValue.get<int32_t>();
        } else if (HEDLEY_UNLIKELY(jsonValue.is_boolean())) {
            type = CHelper::BlockStateType::BOOLEAN;
            value = jsonValue.get<bool>();
        } else {
            type = CHelper::BlockStateType::STRING;
            value = jsonValue.get<std::string>();
        }
        node = getNodeValue(type, value, description);
    }

    BlockStateValue::BlockStateValue(const BlockStateValue &blockStateValue)
            : type(blockStateValue.type),
              value(blockStateValue.value),
              description(blockStateValue.description),
              node(getNodeValue(type, value, description)) {}

    BlockStateValue::~BlockStateValue() {
        delete node;
    }

    BlockStateValue &BlockStateValue::operator=(const BlockStateValue &blockStateValue) {
        if (HEDLEY_UNLIKELY(this == &blockStateValue)) {
            return *this;
        }
        delete node;
        type = blockStateValue.type;
        value = blockStateValue.value;
        description = blockStateValue.description;
        node = getNodeValue(type, value, description);
        return *this;
    }

    void BlockStateValue::toJson(nlohmann::json &j) const {
        switch (type) {
            case CHelper::BlockStateType::STRING:
                JsonUtil::toJson(j, "value", std::get<std::string>(value));
                break;
            case CHelper::BlockStateType::BOOLEAN:
                JsonUtil::toJson(j, "value", std::get<bool>(value));
                break;
            case CHelper::BlockStateType::INTEGER:
                JsonUtil::toJson(j, "value", std::get<int32_t>(value));
                break;
        }
        JsonUtil::toJsonOptionalLikely(j, "description", description);
    }

    static Node::NodeBase *
    getNodePerBlockState(const std::string &key,
                         const std::optional<std::string> &description,
                         const std::vector<BlockStateValue> &values) {
        std::vector<const Node::NodeBase *> valueNodes;
        valueNodes.reserve(values.size());
        for (const auto &item: values) {
            valueNodes.push_back(item.node);
        }
        //key=value
        return new Node::NodeEntry(
                "BLOCK_STATE_PER_ENTRY", "方块状态单个键值对",
                new Node::NodeText("BLOCK_STATE_ENTRY_KEY", "方块状态键值对的键",
                                   std::make_shared<NormalId>(key, description)),
                nodeBlockStateEntrySeparator.get(),
                new Node::NodeOr("BLOCK_STATE_ENTRY_VALUE", "方块状态键值对的值",
                                 std::move(valueNodes), false));
    }

    BlockState::BlockState(std::string key,
                           const std::optional<std::string> &description,
                           std::vector<BlockStateValue> values,
                           int32_t defaultValue)
            : key(std::move(key)),
              description(description),
              values(std::move(values)),
              defaultValue(defaultValue),
              node(getNodePerBlockState(this->key, description, this->values)) {}

    BlockState::BlockState(const nlohmann::json &j)
            : key(JsonUtil::fromJson<std::string>(j, "key")),
              description(JsonUtil::fromJsonOptionalLikely<std::string>(j, "description")),
              values(JsonUtil::fromJson<std::vector<CHelper::BlockStateValue>>(j, "values")),
              defaultValue(JsonUtil::fromJson<int32_t>(j, "defaultValue")),
              node(getNodePerBlockState(key, description, values)) {}

    BlockState::BlockState(const BlockState &blockState)
            : key(blockState.key),
              description(blockState.description),
              values(blockState.values),
              defaultValue(blockState.defaultValue),
              node(getNodePerBlockState(key, description, values)) {}

    BlockState::~BlockState() {
        delete ((Node::NodeEntry *) node)->nodeKey;
        delete ((Node::NodeEntry *) node)->nodeValue;
        delete node;
    }

    BlockState &BlockState::operator=(const BlockState &blockState) {
        if (HEDLEY_UNLIKELY(this == &blockState)) {
            return *this;
        }
        delete ((Node::NodeEntry *) node)->nodeKey;
        delete ((Node::NodeEntry *) node)->nodeValue;
        delete node;
        key = blockState.key;
        description = blockState.description;
        values = blockState.values;
        defaultValue = blockState.defaultValue;
        node = getNodePerBlockState(key, description, values);
        return *this;
    }

    void BlockState::toJson(nlohmann::json &j) const {
        JsonUtil::toJson(j, "key", key);
        JsonUtil::toJsonOptionalLikely(j, "description", description);
        JsonUtil::toJson(j, "values", values);
        JsonUtil::toJson(j, "defaultValue", defaultValue);
    }

    static Node::NodeBase *
    getNodeBlockState(const std::optional<std::vector<BlockState>> &blockStates) {
        std::vector<const Node::NodeBase *> blockStateEntryChildNode2;
        //已知的方块状态
        if (HEDLEY_LIKELY(blockStates.has_value())) {
            blockStateEntryChildNode2.reserve(2);
            std::vector<const Node::NodeBase *> blockStateEntryChildNode1;
            blockStateEntryChildNode1.reserve(blockStates->size());
            std::transform(blockStates->begin(), blockStates->end(),
                           std::back_inserter(blockStateEntryChildNode1),
                           [](const auto &item) -> const Node::NodeBase * {
                               return item.node;
                           });
            blockStateEntryChildNode2.push_back(new Node::NodeOr(
                    "BLOCK_STATE_ENTRY", "方块状态键值对",
                    std::move(blockStateEntryChildNode1), false));
        }
        //其他未知的方块状态
        blockStateEntryChildNode2.push_back(nodeBlockStateAllEntry.get());
        //把所有方块状态拼在一起
        return new Node::NodeList(
                "BLOCK_STATE", "方块状态",
                nodeBlockStateLeftBracket.get(),
                new Node::NodeOr(
                        "BLOCK_STATE_ENTRY", "方块状态键值对",
                        std::move(blockStateEntryChildNode2), false, true),
                nodeBlockStateSeparator.get(),
                nodeBlockStateRightBracket.get());
    }

    BlockId::BlockId(const std::optional<std::string> &nameSpace,
                     const std::string &name,
                     const std::optional<std::string> &description,
                     const std::optional<int32_t> &max,
                     const std::optional<std::vector<std::string>> &descriptions,
                     const std::optional<std::vector<BlockState>> &blockStates)
            : ItemId(nameSpace, name, description, max, descriptions),
              blockStates(blockStates),
              nodeBlockState(getNodeBlockState(blockStates)) {}

    BlockId::BlockId(const nlohmann::json &j)
            : ItemId(j),
              blockStates(JsonUtil::fromJsonOptionalLikely<std::vector<CHelper::BlockState>>(j, "blockStates")),
              nodeBlockState(getNodeBlockState(blockStates)) {}

    BlockId::BlockId(const BlockId &blockId)
            : ItemId(blockId.nameSpace, blockId.name, blockId.description, blockId.max, blockId.descriptions),
              blockStates(blockId.blockStates),
              nodeBlockState(getNodeBlockState(blockStates)) {}

    BlockId::~BlockId() {
        if (HEDLEY_LIKELY(blockStates.has_value())) {
            delete ((Node::NodeOr *) ((Node::NodeList *) nodeBlockState)->nodeElement)->childNodes[0];
        }
        delete ((Node::NodeList *) nodeBlockState)->nodeElement;
        delete nodeBlockState;
    }

    void BlockId::toJson(nlohmann::json &j) const {
        ItemId::toJson(j);
        JsonUtil::toJsonOptionalLikely(j, "blockStates", blockStates);
    }

    Node::NodeBase *BlockId::getNodeAllBlockState() {
        return nodeAllBlockState.get();
    }

} // CHelper