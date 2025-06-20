//
// Created by Yancey on 2024-12-20.
//

#include <chelper/node/NodeType.h>
#include <chelper/node/util/NodeWrapped.h>

namespace CHelper::Node {

    NodeWrapped::NodeWrapped(NodeSerializable *innerNode)
        : innerNode(innerNode) {}

    void NodeWrapped::init(const CPack &cpack) {
        innerNode->init(cpack);
    }

    NodeTypeId::NodeTypeId NodeWrapped::getNodeType() const {
        return NodeTypeId::WRAPPED;
    }

}// namespace CHelper::Node