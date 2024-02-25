//
// Created by Yancey on 2023/12/18.
//

#include "NodeBlockState.h"
#include "../util/NodeSingleSymbol.h"
#include "../util/NodeList.h"
#include "../param/NodeString.h"
#include "../util/NodeEntry.h"
#include "../util/NodeOr.h"
#include "../param/NodeBoolean.h"
#include "../param/NodeInteger.h"
#include "../param/NodeFloat.h"

namespace CHelper::Node {

    NodeBlockState::NodeBlockState(const std::optional<std::string> &id,
                                   const std::optional<std::string> &description)
            : NodeBase(id, description, false) {}

    std::shared_ptr<NodeBlockState> NodeBlockState::getInstance() {
        static std::shared_ptr<NodeBlockState> INSTANCE = std::make_shared<NodeBlockState>("BLOCK_STATE", "方块状态");
        return INSTANCE;
    }


    static std::shared_ptr<NodeBase> nodeLeftBracket =
            std::make_shared<NodeSingleSymbol>("BLOCK_STATE_LEFT_BRACKET",
                                               "方块状态左括号",
                                               '[');
    static std::shared_ptr<NodeBase> nodeEntryKey =
            std::make_shared<NodeString>("BLOCK_STATE_ENTRY_KEY",
                                         "方块状态键值对的键",
                                         true,
                                         false);
    static std::shared_ptr<NodeBase> nodeEntrySeparator =
            std::make_shared<NodeSingleSymbol>(
                    "BLOCK_STATE_ENTRY_SEPARATOR",
                    "方块状态键值对分隔符",
                    '=');
    static std::shared_ptr<NodeBase> nodeEntryValue =
            std::make_shared<NodeOr>("BLOCK_STATE_ENTRY_VALUE",
                                     "方块状态键值对的值",
                                     std::make_shared<std::vector<std::shared_ptr<NodeBase>>>(
                                             std::vector<std::shared_ptr<NodeBase>>{
                                                     std::make_shared<NodeBoolean>(
                                                             "BLOCK_STATE_ENTRY_VALUE_BOOLEAN",
                                                             "方块状态键值对的值（布尔值）",
                                                             std::nullopt,
                                                             std::nullopt),
                                                     std::make_shared<NodeInteger>(
                                                             "BLOCK_STATE_ENTRY_VALUE_INTEGER",
                                                             "方块状态键值对的值（整数）",
                                                             std::nullopt,
                                                             std::nullopt),
                                                     std::make_shared<NodeFloat>(
                                                             "BLOCK_STATE_ENTRY_VALUE_FLOAT",
                                                             "方块状态键值对的值（小数）",
                                                             std::nullopt,
                                                             std::nullopt),
                                                     std::make_shared<NodeString>(
                                                             "BLOCK_STATE_ENTRY_VALUE_STRING",
                                                             "方块状态键值对的值（字符串）",
                                                             true,
                                                             false)
                                             }
                                     ),
                                     false
            );
    static std::shared_ptr<NodeBase> nodeEntry =
            std::make_shared<NodeEntry>("BLOCK_STATE_ENTRY",
                                        "方块状态键值对",
                                        nodeEntryKey,
                                        nodeEntrySeparator,
                                        nodeEntryValue);
    static std::shared_ptr<NodeBase> nodeSeparator =
            std::make_shared<NodeSingleSymbol>("BLOCK_STATE_SEPARATOR",
                                               "方块状态分隔符",
                                               ',');
    static std::shared_ptr<NodeBase> nodeRightBracket =
            std::make_shared<NodeSingleSymbol>("BLOCK_STATE_RIGHT_BRACKET",
                                               "方块状态右括号",
                                               ']');
    static std::shared_ptr<NodeBase> nodeBlockState =
            std::make_shared<NodeList>("BLOCK_STATE",
                                       "方块状态",
                                       nodeLeftBracket,
                                       nodeEntry,
                                       nodeSeparator,
                                       nodeRightBracket);

    ASTNode NodeBlockState::getASTNode(TokenReader &tokenReader) const {
        return getByChildNode(tokenReader, nodeBlockState, "blockState");
    }

    void NodeBlockState::collectStructure(const ASTNode *astNode,
                                          StructureBuilder &structure,
                                          bool isMustHave) const {
        if (astNode == nullptr || astNode->id == "blockState") {
            structure.append(true, "方块状态");
        }
    }

} // CHelper::Node