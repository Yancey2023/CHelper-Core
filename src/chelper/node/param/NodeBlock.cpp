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
            : NodeBase(id, description),
              nodeBlockType(nodeBlockType),
              nodeBlockId(std::make_shared<NodeNamespaceId>("BLOCK_ID", "方块ID", "blocks", contents)) {}

    NodeBlock::NodeBlock(const nlohmann::json &j,
                         const CPack &cpack) :
            NodeBase(j, cpack),
            nodeBlockType(FROM_JSON(j, nodeBlockType, NodeBlockType::NodeBlockType)),
            nodeBlockId(std::make_shared<NodeNamespaceId>("BLOCK_ID", "方块ID", "blocks", cpack.blockIds)) {}

    NodeType NodeBlock::getNodeType() const {
        return NodeType::BLOCK;
    }

    void NodeBlock::toJson(nlohmann::json &j) const {
        NodeBase::toJson(j);
        TO_JSON(j, nodeBlockType);
    }

    ASTNode NodeBlock::getASTNode(TokenReader &tokenReader) const {
        return getOptionalASTNode(tokenReader, false,
                                  {nodeBlockId, NodeBlockState::getInstance()});
    }

    std::optional<std::string> NodeBlock::collectDescription(const ASTNode *node, size_t index) const {
        return std::nullopt;
    }

} // CHelper::Node