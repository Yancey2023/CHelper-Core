//
// Created by Yancey on 2023/11/11.
//

#include "NodeBlock.h"
#include "../block/NodeBlockState.h"

namespace CHelper::Node {

    NodeBlock::NodeBlock(const std::optional<std::string> &id,
                         const std::optional<std::string> &description,
                         NodeBlockType::NodeBlockType nodeBlockType,
                         const std::shared_ptr<std::vector<std::shared_ptr<NamespaceId>>> &contents)
            : NodeBase(id, description, false),
              nodeBlockType(nodeBlockType),
              nodeBlockId(std::make_shared<NodeNamespaceId>("BLOCK_ID", "方块ID", "blocks", contents)) {}

    NodeBlock::NodeBlock(const nlohmann::json &j,
                         const CPack &cpack) :
            NodeBase(j, true),
            nodeBlockType(FROM_JSON(j, nodeBlockType, NodeBlockType::NodeBlockType)),
            nodeBlockId(std::make_shared<NodeNamespaceId>("BLOCK_ID", "方块ID", "blocks", cpack.blockIds)) {}

    std::shared_ptr<NodeType> NodeBlock::getNodeType() const {
        return NodeType::BLOCK;
    }

    void NodeBlock::toJson(nlohmann::json &j) const {
        NodeBase::toJson(j);
        TO_JSON(j, nodeBlockType);
    }

    ASTNode NodeBlock::getASTNode(TokenReader &tokenReader) const {
        switch (nodeBlockType) {
            case NodeBlockType::BLOCK_WITH_BLOCK_STATE:
                return getOptionalASTNode(tokenReader, false,
                                          {nodeBlockId, NodeBlockState::getInstance()});
            case NodeBlockType::BLOCK:
                return getByChildNode(tokenReader, nodeBlockId);
            default:
                return getOptionalASTNode(tokenReader, false,
                                          {nodeBlockId, NodeBlockState::getInstance()});
        }
    }

    std::optional<std::string> NodeBlock::collectDescription(const ASTNode *node, size_t index) const {
        return std::nullopt;
    }

    void NodeBlock::collectStructure(const ASTNode *astNode, StructureBuilder &structure, bool isMustHave) const {
        switch (nodeBlockType) {
            case NodeBlockType::BLOCK_WITH_BLOCK_STATE:
                nodeBlockId->collectStructure(nullptr, structure, isMustHave);
                NodeBlockState::getInstance()->collectStructure(nullptr, structure, false);
                break;
            case NodeBlockType::BLOCK:
                nodeBlockId->collectStructure(nullptr, structure, isMustHave);
                break;
            default:
                nodeBlockId->collectStructure(nullptr, structure, isMustHave);
                NodeBlockState::getInstance()->collectStructure(nullptr, structure, false);
                break;
        }

    }

} // CHelper::Node