//
// Created by Yancey on 2025-06-21.
//

#include <chelper/node/NodeType.h>
#include <chelper/node/NodeWithType.h>

#define CHELPER_DELETE(v1)                                                           \
    case Node::NodeTypeId::v1:                                                       \
        delete static_cast<NodeTypeDetail<Node::NodeTypeId::v1>::Type *>(item.data); \
        break;

namespace CHelper::Node {

    FreeableNodeWithTypes::~FreeableNodeWithTypes() {
        for (auto &item: nodes) {
            if (item.data == nullptr) {
                continue;
            }
            switch (item.nodeTypeId) {
                CODEC_PASTE(CHELPER_DELETE, CHELPER_NODE_TYPES);
            }
            item.data = nullptr;
        }
    }

}// namespace CHelper::Node
