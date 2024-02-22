//
// Created by Yancey666 on 2023/11/11.
//

#include "NodeBlock.h"
#include "../block/NodeBlockName.h"
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
                                  {NodeBlockName::getInstance(), NodeBlockState::getInstance()});
    }

} // CHelper::Node