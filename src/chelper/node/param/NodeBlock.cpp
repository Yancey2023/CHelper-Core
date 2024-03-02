//
// Created by Yancey on 2023/11/11.
//

#include "NodeBlock.h"
#include "../util/NodeSingleSymbol.h"
#include "NodeString.h"
#include "NodeBoolean.h"
#include "NodeInteger.h"
#include "NodeFloat.h"
#include "../util/NodeEntry.h"
#include "../util/NodeList.h"
#include "NodeText.h"
#include "../util/NodeAnd.h"
#include "../../util/TokenUtil.h"

namespace CHelper::Node {

    static std::shared_ptr<NodeBase> nodeBlockStateLeftBracket = std::make_shared<NodeSingleSymbol>(
            "BLOCK_STATE_LEFT_BRACKET", "方块状态左括号", '[');
    static std::shared_ptr<NodeBase> nodeBlockStateEntryKey = std::make_shared<NodeString>(
            "BLOCK_STATE_ENTRY_KEY", "方块状态键值对的键", false, false, false);
    static std::shared_ptr<NodeBase> nodeBlockStateEntrySeparator = std::make_shared<NodeSingleSymbol>(
            "BLOCK_STATE_ENTRY_SEPARATOR", "方块状态键值对分隔符", '=');
    static std::shared_ptr<NodeBase> nodeBlockStateEntryAllValue = std::make_shared<NodeOr>(
            "BLOCK_STATE_ENTRY_VALUE", "方块状态键值对的值",
            std::make_shared<std::vector<std::shared_ptr<NodeBase>>>(
                    std::vector<std::shared_ptr<NodeBase>>{
                            std::make_shared<NodeBoolean>(
                                    "BLOCK_STATE_ENTRY_VALUE_BOOLEAN", "方块状态键值对的值（布尔值）",
                                    std::nullopt, std::nullopt),
                            std::make_shared<NodeInteger>(
                                    "BLOCK_STATE_ENTRY_VALUE_INTEGER", "方块状态键值对的值（整数）",
                                    std::nullopt, std::nullopt),
                            std::make_shared<NodeFloat>(
                                    "BLOCK_STATE_ENTRY_VALUE_FLOAT", "方块状态键值对的值（小数）",
                                    std::nullopt, std::nullopt),
                            std::make_shared<NodeString>(
                                    "BLOCK_STATE_ENTRY_VALUE_STRING", "方块状态键值对的值（字符串）",
                                    false, true, false)
                    }
            ),
            false
    );
    static std::shared_ptr<NodeBase> nodeBlockStateAllEntry = std::make_shared<NodeEntry>(
            "BLOCK_STATE_ENTRY", "方块状态键值对", nodeBlockStateEntryKey,
            nodeBlockStateEntrySeparator, nodeBlockStateEntryAllValue);
    static std::shared_ptr<NodeBase> nodeBlockStateSeparator = std::make_shared<NodeSingleSymbol>(
            "BLOCK_STATE_SEPARATOR", "方块状态分隔符", ',');
    static std::shared_ptr<NodeBase> nodeBlockStateRightBracket = std::make_shared<NodeSingleSymbol>(
            "BLOCK_STATE_RIGHT_BRACKET", "方块状态右括号", ']');
    static std::shared_ptr<NodeBase> nodeAllBlockState = std::make_shared<NodeList>(
            "BLOCK_STATE", "方块状态",
            nodeBlockStateLeftBracket, nodeBlockStateAllEntry,
            nodeBlockStateSeparator, nodeBlockStateRightBracket);

    NodeBlock::NodeBlock(const std::optional<std::string> &id,
                         const std::optional<std::string> &description,
                         NodeBlockType::NodeBlockType nodeBlockType,
                         const std::shared_ptr<std::vector<std::shared_ptr<NamespaceId>>> &contents)
            : NodeBase(id, description, false),
              nodeBlockType(nodeBlockType),
              blockIds(contents),
              nodeBlockId(std::make_shared<NodeNamespaceId>("BLOCK_ID", "方块ID", "blocks", contents)) {}

    NodeBlock::NodeBlock(const nlohmann::json &j,
                         const CPack &cpack) :
            NodeBase(j, true),
            nodeBlockType(FROM_JSON(j, nodeBlockType, NodeBlockType::NodeBlockType)),
            blockIds(cpack.blockIds),
            nodeBlockId(std::make_shared<NodeNamespaceId>("BLOCK_ID", "方块ID", "blocks", cpack.blockIds)) {}

    std::shared_ptr<NodeType> NodeBlock::getNodeType() const {
        return NodeType::BLOCK;
    }

    void NodeBlock::toJson(nlohmann::json &j) const {
        NodeBase::toJson(j);
        TO_JSON(j, nodeBlockType);
    }

    ASTNode NodeBlock::getASTNode(TokenReader &tokenReader, const CPack &cpack) const {
        tokenReader.push();
        ASTNode blockId = getByChildNode(tokenReader, cpack, nodeBlockId);
        if (nodeBlockType == NodeBlockType::BLOCK || blockId.isError()) {
            tokenReader.pop();
            return blockId;
        }
        //查找方块ID
        std::string blockIdStr = TokenUtil::toString(blockId.tokens);
        std::string_view nameSpace = "minecraft";
        std::string_view value = blockIdStr;
        size_t index = blockIdStr.find(':');
        if (index != std::string::npos) {
            nameSpace = value.substr(0, index);
            value = value.substr(index + 1);
        }
        std::shared_ptr<NamespaceId> currentBlock = nullptr;
        for (const auto &item: *blockIds) {
            if (value == item->name && nameSpace == item->nameSpace.value_or("minecraft")) {
                currentBlock = item;
                break;
            }
        }
        if (currentBlock == nullptr) {
            //找不到方块ID
            auto blockState = nodeAllBlockState->getASTNode(tokenReader, cpack);
            return ASTNode::andNode(this, {blockId, blockState}, tokenReader.collect());
        }
        //找到方块ID
        auto nodeBlockState = currentBlock == nullptr ? nodeAllBlockState :
                              std::static_pointer_cast<BlockId>(currentBlock)->nodeBlockState;
        auto astNodeBlockState = nodeBlockState->getASTNode(tokenReader, cpack);
        return ASTNode::andNode(this, {blockId, astNodeBlockState}, tokenReader.collect());
    }

    std::optional<std::string> NodeBlock::collectDescription(const ASTNode *node, size_t index) const {
        if (nodeBlockType == NodeBlockType::BLOCK) {
            return "方块ID";
        } else {
            return "方块ID+方块状态";
        }
    }

    void NodeBlock::collectStructure(const ASTNode *astNode, StructureBuilder &structure, bool isMustHave) const {
        structure.append(isMustHave, "方块ID");
        if (nodeBlockType == NodeBlockType::BLOCK_WITH_BLOCK_STATE) {
            structure.append(false, "方块状态");
        }
    }

} // CHelper::Node