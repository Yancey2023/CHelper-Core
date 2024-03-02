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

    static std::shared_ptr<Node::NodeBase> nodeBlockStateLeftBracket = std::make_shared<Node::NodeSingleSymbol>(
            "BLOCK_STATE_LEFT_BRACKET", "方块状态左括号", '[');
    static std::shared_ptr<Node::NodeBase> nodeBlockStateEntryKey = std::make_shared<Node::NodeString>(
            "BLOCK_STATE_ENTRY_KEY", "方块状态键值对的键", false, false, false);
    static std::shared_ptr<Node::NodeBase> nodeBlockStateEntrySeparator = std::make_shared<Node::NodeSingleSymbol>(
            "BLOCK_STATE_ENTRY_SEPARATOR", "方块状态键值对分隔符", '=');
    static std::shared_ptr<Node::NodeBase> nodeBlockStateEntryAllValue = std::make_shared<Node::NodeOr>(
            "BLOCK_STATE_ENTRY_VALUE", "方块状态键值对的值",
            std::make_shared<std::vector<std::shared_ptr<Node::NodeBase>>>(
                    std::vector<std::shared_ptr<Node::NodeBase>>{
                            std::make_shared<Node::NodeBoolean>(
                                    "BLOCK_STATE_ENTRY_VALUE_BOOLEAN", "方块状态键值对的值（布尔值）",
                                    std::nullopt, std::nullopt),
                            std::make_shared<Node::NodeInteger>(
                                    "BLOCK_STATE_ENTRY_VALUE_INTEGER", "方块状态键值对的值（整数）",
                                    std::nullopt, std::nullopt),
                            std::make_shared<Node::NodeFloat>(
                                    "BLOCK_STATE_ENTRY_VALUE_FLOAT", "方块状态键值对的值（小数）",
                                    std::nullopt, std::nullopt),
                            std::make_shared<Node::NodeString>(
                                    "BLOCK_STATE_ENTRY_VALUE_STRING", "方块状态键值对的值（字符串）",
                                    false, true, false)
                    }
            ),
            false
    );
    static std::shared_ptr<Node::NodeBase> nodeBlockStateAllEntry = std::make_shared<Node::NodeEntry>(
            "BLOCK_STATE_ENTRY", "方块状态键值对", nodeBlockStateEntryKey,
            nodeBlockStateEntrySeparator, nodeBlockStateEntryAllValue);
    static std::shared_ptr<Node::NodeBase> nodeBlockStateSeparator = std::make_shared<Node::NodeSingleSymbol>(
            "BLOCK_STATE_SEPARATOR", "方块状态分隔符", ',');
    static std::shared_ptr<Node::NodeBase> nodeBlockStateRightBracket = std::make_shared<Node::NodeSingleSymbol>(
            "BLOCK_STATE_RIGHT_BRACKET", "方块状态右括号", ']');

    static std::shared_ptr<Node::NodeBase> getNodeValue(BlockStateType::BlockStateType type,
                                                        const std::variant<std::string, int, bool> &value,
                                                        const std::optional<std::string> &description) {
        switch (type) {
            case BlockStateType::STRING:
                return std::make_shared<Node::NodeText>(
                        "BLOCK_STATE_ENTRY_VALUE_STRING", "方块状态键值对的键（字符串）",
                        std::make_shared<NormalId>(std::get<std::string>(value), description));
            case BlockStateType::INTEGER:
                return std::make_shared<Node::NodeText>(
                        "BLOCK_STATE_ENTRY_VALUE_INTEGER", "方块状态键值对的键（整数）",
                        std::make_shared<NormalId>(std::to_string(std::get<int>(value)), description));
            case BlockStateType::BOOLEAN:
                return std::make_shared<Node::NodeText>(
                        "BLOCK_STATE_ENTRY_VALUE_BOOLEAN", "方块状态键值对的键（布尔值）",
                        std::make_shared<NormalId>(std::get<bool>(value) ? "true" : "false", description));
            default:
                return nullptr;
        }
    }

    BlockStateValue::BlockStateValue(BlockStateType::BlockStateType type,
                                     const std::variant<std::string, int, bool> &value,
                                     const std::optional<std::string> &description)
            : type(type),
              value(value),
              description(description),
              node(getNodeValue(type, value, description)) {}

    BlockStateValue::BlockStateValue(const nlohmann::json &j)
            : description(FROM_JSON_OPTIONAL(j, description, std::string)) {
        const nlohmann::json &jsonValue = j.at("value");
        if (jsonValue.is_number_integer()) {
            type = CHelper::BlockStateType::INTEGER;
            value = jsonValue.get<int>();
        } else if (jsonValue.is_boolean()) {
            type = CHelper::BlockStateType::BOOLEAN;
            value = jsonValue.get<bool>();
        } else {
            type = CHelper::BlockStateType::STRING;
            value = jsonValue.get<std::string>();
        }
        node = getNodeValue(type, value, description);
    }

    void BlockStateValue::toJson(nlohmann::json &j) const {
        switch (type) {
            case CHelper::BlockStateType::STRING:
                j["value"] = std::get<std::string>(value);
                break;
            case CHelper::BlockStateType::BOOLEAN:
                j["value"] = std::get<bool>(value);
                break;
            case CHelper::BlockStateType::INTEGER:
                j["value"] = std::get<int>(value);
                break;
        }
        TO_JSON_OPTIONAL(j, description)
    }

    static std::shared_ptr<Node::NodeBase> getNodePerBlockState(const std::string &key,
                                                                const std::optional<std::string> &description,
                                                                const std::vector<BlockStateValue> &values) {
        auto valueNodes = std::make_shared<std::vector<std::shared_ptr<Node::NodeBase>>>();
        for (const auto &item: values) {
            valueNodes->push_back(item.node);
        }
        //key=value
        return std::make_shared<Node::NodeEntry>(
                "BLOCK_STATE_PER_ENTRY", "方块状态单个键值对",
                std::make_shared<Node::NodeText>("BLOCK_STATE_ENTRY_KEY", "方块状态键值对的键",
                                                 std::make_shared<NormalId>(key, description)),
                nodeBlockStateEntrySeparator,
                std::make_shared<Node::NodeOr>("BLOCK_STATE_ENTRY_VALUE", "方块状态键值对的值",
                                               valueNodes, false));
    }

    BlockState::BlockState(std::string key,
                           const std::optional<std::string> &description,
                           const std::vector<BlockStateValue> &values,
                           int defaultValue)
            : key(std::move(key)),
              description(description),
              values(values),
              defaultValue(defaultValue),
              node(getNodePerBlockState(key, description, values)) {}

    BlockState::BlockState(const nlohmann::json &j)
            : key(FROM_JSON(j, key, std::string)),
              description(FROM_JSON_OPTIONAL(j, description, std::string)),
              values(FROM_JSON(j, values, std::vector<CHelper::BlockStateValue>)),
              defaultValue(FROM_JSON(j, defaultValue, int)),
              node(getNodePerBlockState(key, description, values)) {}

    void BlockState::toJson(nlohmann::json &j) const {
        TO_JSON(j, key);
        TO_JSON_OPTIONAL(j, description)
        TO_JSON(j, values);
        TO_JSON(j, defaultValue);
    }

    static std::shared_ptr<Node::NodeBase>
    getNodeBlockState(const std::optional<std::vector<BlockState>> &blockStates) {
        auto blockStateEntryChildNode = std::make_shared<std::vector<std::shared_ptr<Node::NodeBase>>>();
        //已知的方块状态
        if (blockStates.has_value()) {
            for (const auto &item: blockStates.value()) {
                blockStateEntryChildNode->push_back(item.node);
            }
        }
        //其他未知的方块状态
        blockStateEntryChildNode->push_back(std::make_shared<Node::NodeEntry>(
                "BLOCK_STATE_PER_ENTRY", "方块状态单个键值对", nodeBlockStateEntryKey,
                nodeBlockStateEntrySeparator, nodeBlockStateEntryAllValue));
        //把所有方块状态拼在一起
        return std::make_shared<Node::NodeList>(
                "BLOCK_STATE", "方块状态",
                nodeBlockStateLeftBracket,
                std::make_shared<Node::NodeOr>(
                        "BLOCK_STATE_ENTRY", "方块状态键值对",
                        blockStateEntryChildNode, false),
                nodeBlockStateSeparator,
                nodeBlockStateRightBracket);
    }

    BlockId::BlockId(const std::optional<std::string> &nameSpace,
                     const std::string &name,
                     const std::optional<std::string> &description,
                     const std::optional<int> &max,
                     const std::optional<std::vector<std::string>> &descriptions,
                     const std::optional<std::vector<BlockState>> &blockStates)
            : ItemId(nameSpace, name, description, max, descriptions),
              blockStates(blockStates),
              nodeBlockState(getNodeBlockState(blockStates)) {}

    BlockId::BlockId(const nlohmann::json &j)
            : ItemId(j),
              blockStates(FROM_JSON_OPTIONAL(j, blockStates, std::vector<CHelper::BlockState>)),
              nodeBlockState(getNodeBlockState(blockStates)) {}

    void BlockId::toJson(nlohmann::json &j) const {
        TO_JSON_OPTIONAL(j, nameSpace)
        TO_JSON(j, name);
        TO_JSON_OPTIONAL(j, description)
        TO_JSON_OPTIONAL(j, blockStates)
        TO_JSON_OPTIONAL(j, max)
        TO_JSON_OPTIONAL(j, descriptions)
    }

} // CHelper