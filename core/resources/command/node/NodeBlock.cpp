//
// Created by Yancey666 on 2023/11/11.
//

#include "NodeBlock.h"

namespace CHelper::Node {

    NODE_TYPE(BLOCK, NodeBlock)

    NodeBlock::NodeBlock(const std::optional<std::string> &id,
                         const std::optional<std::string> &description,
                         CHelper::Node::NodeBlockType::NodeBlockType nodeBlockType)
            : NodeBase(id, description),
              nodeBlockType(nodeBlockType) {}

    NodeBlock::NodeBlock(const nlohmann::json &j,
                         const CPack &cpack) :
            NodeBase(j, cpack),
            nodeBlockType(FROM_JSON(j, nodeBlockType, CHelper::Node::NodeBlockType::NodeBlockType)) {}

    void NodeBlock::toJson(nlohmann::json &j) const {
        NodeBase::toJson(j);
        TO_JSON(j, nodeBlockType);
    }

} // CHelper::Node