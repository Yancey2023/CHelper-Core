//
// Created by Yancey666 on 2023/11/11.
//

#include "NodeBlock.h"

namespace CHelper::Node {

    NodeBlock::NodeBlock(const std::optional<std::string> &id,
                         const std::optional<std::string> &description,
                         CHelper::Node::NodeBlockType::NodeBlockType nodeBlockType)
            : NodeBase(NodeType::BLOCK, id, description),
              nodeBlockType(nodeBlockType) {}

    NodeBlock::NodeBlock(const nlohmann::json &j) :
            NodeBase(NodeType::BLOCK, j),
            nodeBlockType(FROM_JSON(j, nodeBlockType, CHelper::Node::NodeBlockType::NodeBlockType)) {}

    void NodeBlock::toJson(nlohmann::json &j) const {
        NodeBase::toJson(j);
        j.push_back({"type", NodeType::STR_BLOCK});
        TO_JSON(j, nodeBlockType);
    }

} // CHelper::Node