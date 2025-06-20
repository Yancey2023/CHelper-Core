//
// Created by Yancey on 2023/12/23.
//

#include <chelper/node/NodeType.h>
#include <chelper/node/util/NodeList.h>

namespace CHelper::Node {

    NodeList::NodeList(NodeBase *nodeLeft,
                       NodeBase *nodeElement,
                       NodeBase *nodeSeparator,
                       NodeBase *nodeRight)
        : nodeLeft(nodeLeft),
          nodeElement(nodeElement),
          nodeSeparator(nodeSeparator),
          nodeRight(nodeRight),
          nodeElementOrRight(std::vector<const NodeBase *>{nodeElement, nodeRight}, false),
          nodeSeparatorOrRight(std::vector<const NodeBase *>{nodeSeparator, nodeRight}, false) {
#ifdef CHelperDebug
        if (HEDLEY_UNLIKELY(
                    nodeLeft == nullptr || nodeElement == nullptr || nodeSeparator == nullptr || nodeRight == nullptr)) {
            throw std::runtime_error("NodeOr has a null child node");
        }
#endif
    }

    NodeTypeId::NodeTypeId NodeList::getNodeType() const {
        return NodeTypeId::LIST;
    }

}// namespace CHelper::Node