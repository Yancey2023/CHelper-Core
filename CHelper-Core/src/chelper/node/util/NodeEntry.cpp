//
// Created by Yancey on 2023/12/22.
//

#include <chelper/node/util/NodeEntry.h>

namespace CHelper::Node {

    NodeEntry::NodeEntry(NodeBase *nodeKey,
                         NodeBase *nodeSeparator,
                         NodeBase *nodeValue)
        : nodeKey(nodeKey),
          nodeSeparator(nodeSeparator),
          nodeValue(nodeValue) {}

    NodeTypeId::NodeTypeId NodeEntry::getNodeType() const {
        return NodeTypeId::ENTRY;
    }

}// namespace CHelper::Node