//
// Created by Yancey666 on 2023/12/18.
//

#include "NodeBlockState.h"
#include "../util/NodeSingleSymbol.h"
#include "../util/NodeList.h"
#include "../NodeString.h"
#include "../util/NodeEntry.h"
#include "../util/NodeOr.h"
#include "../NodeBoolean.h"
#include "../NodeInteger.h"
#include "../NodeFloat.h"

namespace CHelper::Node {

    NODE_TYPE_INSTANCE("BLOCK_STATE", NodeBlockState, "方块状态")

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
                                     },
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

    ASTNode NodeBlockState::getASTNode(TokenReader &tokenReader, const CPack &cpack) const {
        return getByChildNode(tokenReader, cpack, nodeBlockState);
    }

} // CHelper::Node