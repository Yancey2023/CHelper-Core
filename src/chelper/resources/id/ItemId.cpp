//
// Created by Yancey on 2023/11/8.
//

#include "ItemId.h"
#include "../../node/param/NodeInteger.h"
#include "../../node/param/NodeText.h"
#include "../../node/util/NodeOr.h"

namespace CHelper {

    std::shared_ptr<Node::NodeBase> ItemId::getNode() {
        if (node == nullptr) {
            if (HEDLEY_UNLIKELY(max.has_value() && max.value() < 0)) {
                throw std::runtime_error("item id max data value should be a positive number");
            }
            auto nodeAllData = std::make_shared<Node::NodeInteger>("ITEM_DATA", "物品附加值", -1, max);
            if (HEDLEY_LIKELY(!descriptions.has_value())) {
                node = nodeAllData;
            } else {
                nodeChildren.push_back(nodeAllData);
                std::vector<const Node::NodeBase *> nodeDataChildren;
                nodeDataChildren.reserve(descriptions.value().size());
                size_t i = 0;
                for (const auto &item: descriptions.value()) {
                    auto nodeChild = std::make_shared<Node::NodeText>(
                            "ITEM_PER_DATA", item,
                            NormalId::make(std::to_string(i++), item),
                            [](const Node::NodeBase *node1, TokenReader &tokenReader) -> ASTNode {
                                return tokenReader.readIntegerASTNode(node1);
                            });
                    nodeDataChildren.push_back(nodeChild.get());
                    nodeChildren.push_back(std::move(nodeChild));
                }
                auto nodeOr = std::make_shared<Node::NodeOr>(
                        "ITEM_DATA", "物品附加值",
                        std::move(nodeDataChildren), false);
                node = std::make_shared<Node::NodeOr>(
                        "ITEM_DATA", "物品附加值",
                        std::vector<const Node::NodeBase *>{nodeOr.get(), nodeAllData.get()},
                        false, true);
                nodeChildren.push_back(std::move(nodeOr));
            }
        }
        return node;
    }

    CODEC(ItemId, idNamespace, name, description, max, descriptions);

}// namespace CHelper