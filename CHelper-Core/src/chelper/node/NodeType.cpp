//
// Created by Yancey on 2023/12/10.
//

#include <chelper/node/NodeType.h>

namespace CHelper::Node {

    const char *getNodeTypeName(const CHelper::Node::NodeTypeId::NodeTypeId id) {
        switch (id) {
            CODEC_PASTE(CHELPER_GET_NAME, CHELPER_NODE_TYPES)
            default:
            return "UNKNOWN";
        }
    }

}// namespace CHelper::Node
