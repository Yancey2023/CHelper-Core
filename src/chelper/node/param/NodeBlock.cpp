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

    NodeBlock::NodeBlock(const std::optional<std::string> &id,
                         const std::optional<std::string> &description,
                         NodeBlockType::NodeBlockType nodeBlockType,
                         const std::shared_ptr<std::vector<std::shared_ptr<NamespaceId>>> &contents)
            : NodeBase(id, description, false),
              nodeBlockType(nodeBlockType),
              blockIds(contents),
              nodeBlockId(std::make_shared<NodeNamespaceId>("BLOCK_ID", "方块ID", "blocks", true, contents)) {}

    NodeBlock::NodeBlock(const nlohmann::json &j,
                         const CPack &cpack) :
            NodeBase(j, true),
            nodeBlockType(JsonUtil::fromJson<NodeBlockType::NodeBlockType>(j, "nodeBlockType")),
            blockIds(cpack.blockIds),
            nodeBlockId(std::make_shared<NodeNamespaceId>("BLOCK_ID", "方块ID", "blocks", true, cpack.blockIds)) {}

    NodeType *NodeBlock::getNodeType() const {
        return NodeType::BLOCK.get();
    }

    void NodeBlock::toJson(nlohmann::json &j) const {
        NodeBase::toJson(j);
        JsonUtil::toJson(j, "nodeBlockType", nodeBlockType);
    }

    ASTNode NodeBlock::getASTNode(TokenReader &tokenReader, const CPack *cpack) const {
        tokenReader.push();
        ASTNode blockId = getByChildNode(tokenReader, cpack, nodeBlockId.get(), "blockId");
        if (nodeBlockType == NodeBlockType::BLOCK || blockId.isError()) {
            tokenReader.pop();
            return blockId;
        }
        std::string blockIdStr = TokenUtil::toString(blockId.tokens);
        size_t strHash = std::hash<std::string>{}(blockIdStr);
        std::shared_ptr<NamespaceId> currentBlock = nullptr;
        for (const auto &item: *blockIds) {
            if (item->fastMatch(strHash) || item->idWithNamespace->fastMatch(strHash)) {
                currentBlock = item;
                break;
            }
        }
        auto nodeBlockState = currentBlock == nullptr ? BlockId::getNodeAllBlockState() :
                              std::static_pointer_cast<BlockId>(currentBlock)->nodeBlockState;
        auto astNodeBlockState = getByChildNode(tokenReader, cpack, nodeBlockState, "blockState");
        return ASTNode::andNode(this, {blockId, astNodeBlockState}, tokenReader.collect());
    }

    std::optional<std::string> NodeBlock::collectDescription(const ASTNode *node, size_t index) const {
        if (node->id == "blockId") {
            return "方块ID";
        } else if (node->id == "blockState") {
            return "方块状态";
        } else {
            return std::nullopt;
        }
    }

    void NodeBlock::collectStructure(const ASTNode *astNode, StructureBuilder &structure, bool isMustHave) const {
        structure.append(isMustHave, "方块ID");
        if (nodeBlockType == NodeBlockType::BLOCK_WITH_BLOCK_STATE) {
            structure.append(false, "方块状态");
        }
    }

} // CHelper::Node