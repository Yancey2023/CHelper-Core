//
// Created by Yancey on 2023/11/11.
//

#include "NodeBlock.h"
#include "../block/NodeBlockId.h"
#include "../block/NodeBlockState.h"

namespace CHelper::Node {

    NODE_TYPE("BLOCK", NodeBlock)

    NodeBlock::NodeBlock(const std::optional<std::string> &id,
                         const std::optional<std::string> &description,
                         NodeBlockType::NodeBlockType nodeBlockType)
            : NodeBase(id, description),
              nodeBlockType(nodeBlockType) {}

    NodeBlock::NodeBlock(const nlohmann::json &j,
                         const CPack &cpack) :
            NodeBase(j, cpack),
            nodeBlockType(FROM_JSON(j, nodeBlockType, NodeBlockType::NodeBlockType)) {}

    void NodeBlock::toJson(nlohmann::json &j) const {
        NodeBase::toJson(j);
        TO_JSON(j, nodeBlockType);
    }

    ASTNode NodeBlock::getASTNode(TokenReader &tokenReader, const CPack &cpack) const {
        return getOptionalASTNode(tokenReader, cpack, false,
                                  {NodeBlockId::getInstance(), NodeBlockState::getInstance()});
    }

    std::optional<std::string> NodeBlock::collectDescription(const ASTNode *node, size_t index) const {
        return std::nullopt;
    }

} // CHelper::Node