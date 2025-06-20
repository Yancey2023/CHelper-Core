//
// Created by Yancey on 2023/12/22.
//

#include <chelper/node/util/NodeAnd.h>
#include <chelper/node/util/NodeWrapped.h>

namespace CHelper::Node {

    NodeAnd::NodeAnd(const std::vector<const NodeBase *> &childNodes)
        : childNodes(childNodes) {}

    NodeTypeId::NodeTypeId NodeAnd::getNodeType() const {
        return NodeTypeId::AND;
    }

}// namespace CHelper::Node