//
// Created by Yancey on 2023/12/10.
//

#include <chelper/node/NodeType.h>

#define CHELPER_GET_NAME(v1) \
    case NodeTypeId::v1:     \
        return NodeTypeDetail<CHelper::Node::NodeTypeId::v1>::name;

#define CHELPER_GET_NODE_TYPE_BY_NAME(v1)                                             \
    if (CHelper::Node::NodeTypeDetail<CHelper::Node::NodeTypeId::v1>::name == type) { \
        return CHelper::Node::NodeTypeId::v1;                                         \
    }

namespace CHelper::Node {

    const char *getNodeTypeName(const CHelper::Node::NodeTypeId::NodeTypeId id) {
        switch (id) {
            CODEC_PASTE(CHELPER_GET_NAME, CHELPER_NODE_TYPES)
            default:
                return "UNKNOWN";
        }
    }

    std::optional<CHelper::Node::NodeTypeId::NodeTypeId> getNodeTypeIdByName(const std::string_view &type) {
        CODEC_PASTE(CHELPER_GET_NODE_TYPE_BY_NAME, CHELPER_NODE_TYPES)
        return std::nullopt;
    }

}// namespace CHelper::Node
