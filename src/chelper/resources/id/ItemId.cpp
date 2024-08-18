//
// Created by Yancey on 2023/11/8.
//

#include "ItemId.h"
#include "../../node/param/NodeInteger.h"
#include "../../node/param/NodeText.h"
#include "../../node/util/NodeOr.h"

namespace CHelper {

    std::shared_ptr<Node::NodeBase> ItemId::getNode() {
        if (HEDLEY_UNLIKELY(node == nullptr)) {
            if (HEDLEY_UNLIKELY(max.has_value() && max.value() < 0)) {
                throw std::runtime_error("item id max data value should be a positive number");
            }
            auto nodeAllData = std::make_shared<Node::NodeInteger>(L"ITEM_DATA", L"物品附加值", -1, max);
            if (HEDLEY_LIKELY(!descriptions.has_value())) {
                node = nodeAllData;
            } else {
                nodeChildren.push_back(nodeAllData);
                std::vector<const Node::NodeBase *> nodeDataChildren;
                nodeDataChildren.reserve(descriptions.value().size());
                size_t i = 0;
                for (const auto &item: descriptions.value()) {
                    auto nodeChild = std::make_shared<Node::NodeText>(
                            L"ITEM_PER_DATA", item,
                            NormalId::make(std::to_wstring(i++), item),
                            [](const Node::NodeBase *node1, TokenReader &tokenReader) -> ASTNode {
                                return tokenReader.readIntegerASTNode(node1);
                            });
                    nodeDataChildren.push_back(nodeChild.get());
                    nodeChildren.push_back(std::move(nodeChild));
                }
                auto nodeOr = std::make_shared<Node::NodeOr>(
                        L"ITEM_DATA", L"物品附加值",
                        std::move(nodeDataChildren), false);
                node = std::make_shared<Node::NodeOr>(
                        L"ITEM_DATA", L"物品附加值",
                        std::vector<const Node::NodeBase *>{nodeOr.get(), nodeAllData.get()},
                        false, true);
                nodeChildren.push_back(std::move(nodeOr));
            }
        }
        return node;
    }

    CODEC_WITH_PARENT(ItemId, NamespaceId, max, descriptions)

}// namespace CHelper