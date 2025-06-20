//
// Created by Yancey on 2023/11/11.
//

#include <chelper/node/param/NodeBlock.h>
#include <chelper/node/util/NodeSingleSymbol.h>

namespace CHelper::Node {

    std::unique_ptr<Node::NodeSingleSymbol> NodeBlock::nodeBlockStateLeftBracket = std::make_unique<Node::NodeSingleSymbol>(u'[', u"方块状态左括号");

    void NodeBlock::init(const CPack &cpack) {
        blockIds = cpack.blockIds;
        nodeBlockId = std::make_shared<NodeNamespaceId>("BLOCK_ID", u"方块ID", "block", true);
        nodeBlockId->init(cpack);
    }

    NodeTypeId::NodeTypeId NodeBlock::getNodeType() const {
        return NodeTypeId::BLOCK;
    }

}// namespace CHelper::Node