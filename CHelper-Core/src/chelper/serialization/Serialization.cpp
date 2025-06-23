//
// Created by Yancey on 2025-06-20.
//

#include <chelper/serialization/Serialization.h>

#define CHELPER_GET_NODE_TYPE_BY_NAME(v1)                                               \
    if (CHelper::Node::NodeTypeDetail<CHelper::Node::NodeTypeId::v1>::name == type) { \
        return CHelper::Node::NodeTypeId::v1;                                           \
    }

namespace CHelper::Serialization {

}// namespace CHelper::Serialization

std::optional<CHelper::Node::NodeTypeId::NodeTypeId> NodeTypeHelper::getNodeTypeIdByName(const std::string_view &type) {
    CODEC_PASTE(CHELPER_GET_NODE_TYPE_BY_NAME, CHELPER_NODE_TYPES)
    return std::nullopt;
}
