//
// Created by Yancey on 2023/11/11.
//

#include <chelper/node/param/NodeItem.h>
#include <chelper/node/param/NodeJson.h>
#include <chelper/node/template/NodeTemplateNumber.h>
#include <chelper/resources/CPack.h>

#ifdef CHelperDebug
#include <chelper/node/NodeType.h>
#endif

namespace CHelper::Node {

    std::shared_ptr<NodeBase> NodeItem::nodeCount = NodeInteger::make("ITEM_COUNT", u"物品数量", 0, std::nullopt);
    std::shared_ptr<NodeBase> NodeItem::nodeAllData = NodeInteger::make("ITEM_DATA", u"物品附加值", -1, std::nullopt);

    void NodeItem::init(const CPack &cpack) {
        itemIds = cpack.itemIds;
        nodeItemId = std::make_unique<NodeNamespaceId>("ITEM_ID", u"物品ID", "item", true);
        nodeComponent = std::make_unique<NodeJson>("ITEM_COMPONENT", u"物品组件", "components");
        nodeItemId->init(cpack);
        nodeComponent->init(cpack);
    }

    NodeTypeId::NodeTypeId NodeItem::getNodeType() const {
        return NodeTypeId::ITEM;
    }

}// namespace CHelper::Node