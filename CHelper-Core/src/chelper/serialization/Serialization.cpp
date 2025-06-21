//
// Created by Yancey on 2025-06-20.
//

#include <chelper/serialization/Serialization.h>

namespace CHelper::Serialization {

}// namespace CHelper::Serialization

const char *NodeTypeHelper::getName(const CHelper::Node::NodeTypeId::NodeTypeId id) {
    switch (id) {
        CODEC_PASTE(CHELPER_GET_NAME, CHELPER_NODE_TYPES)
        default:
            return "UNKNOWN";
    }
}
