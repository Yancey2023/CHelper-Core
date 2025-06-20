//
// Created by Yancey on 2023/11/11.
//

#include <chelper/node/param/NodeBlock.h>
#include <chelper/node/util/NodeSingleSymbol.h>

namespace CHelper::Node {

    std::unique_ptr<Node::NodeSingleSymbol> NodeBlock::nodeBlockStateLeftBracket = std::make_unique<Node::NodeSingleSymbol>(u'[', u"方块状态左括号");

    void NodeBlock::init(const CPack &cpack) {
        blockIds = cpack.blockIds;
        nodeBlockId = std::make_shared<NodeNamespaceId>("BLOCK_ID", u"方块ID", "block", true);
        nodeBlockId->init(cpack);
    }

    NodeTypeId::NodeTypeId NodeBlock::getNodeType() const {
        return NodeTypeId::BLOCK;
    }

    ASTNode NodeBlock::getASTNode(TokenReader &tokenReader, const CPack *cpack) const {
        tokenReader.push();
        ASTNode blockId = getByChildNode(tokenReader, cpack, nodeBlockId.get(), ASTNodeId::NODE_BLOCK_BLOCK_ID);
        if (HEDLEY_UNLIKELY(nodeBlockType == NodeBlockType::BLOCK || blockId.isError())) {
            tokenReader.pop();
            return blockId;
        }
        tokenReader.push();
        ASTNode blockStateLeftBracket = nodeBlockStateLeftBracket->getASTNode(tokenReader, cpack);
        tokenReader.restore();
        if (HEDLEY_LIKELY(blockStateLeftBracket.isError())) {
            return ASTNode::andNode(this, {blockId}, tokenReader.collect(),
                                    nullptr, ASTNodeId::NODE_BLOCK_BLOCK_AND_BLOCK_STATE);
        }
        std::u16string_view str = blockId.tokens.toString();
        XXH64_hash_t strHash = XXH3_64bits(str.data(), str.size() * sizeof(decltype(str)::value_type));
        std::shared_ptr<NamespaceId> currentBlock = nullptr;
        for (const auto &item: *blockIds->blockStateValues) {
            if (HEDLEY_UNLIKELY(item->fastMatch(strHash) || item->getIdWithNamespace()->fastMatch(strHash))) {
                currentBlock = item;
                break;
            }
        }
        auto nodeBlockState = currentBlock == nullptr
                                      ? BlockId::getNodeAllBlockState()
                                      : std::static_pointer_cast<BlockId>(currentBlock)->getNode(blockIds->blockPropertyDescriptions).get();
        auto astNodeBlockState = getByChildNode(tokenReader, cpack, nodeBlockState, ASTNodeId::NODE_BLOCK_BLOCK_STATE);
        return ASTNode::andNode(this, {blockId, astNodeBlockState}, tokenReader.collect(),
                                nullptr, ASTNodeId::NODE_BLOCK_BLOCK_AND_BLOCK_STATE);
    }

    void NodeBlock::collectStructure(const ASTNode *astNode, StructureBuilder &structure, bool isMustHave) const {
        structure.append(isMustHave, u"方块ID");
        if (HEDLEY_LIKELY(nodeBlockType == NodeBlockType::BLOCK_WITH_BLOCK_STATE)) {
            structure.append(false, u"方块状态");
        }
    }

}// namespace CHelper::Node