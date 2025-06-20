//
// Created by Yancey on 2023/11/8.
//

#include <chelper/node/CommandNode.h>
#include <chelper/resources/id/ItemId.h>

namespace CHelper {

    std::shared_ptr<Node::NodeBase> ItemId::getNode() {
        if (HEDLEY_UNLIKELY(node == nullptr)) {
            if (HEDLEY_UNLIKELY(max.has_value() && max.value() < 0)) {
                throw std::runtime_error("item id max data value should be a positive number");
            }
            auto nodeAllData = std::shared_ptr<Node::NodeInteger>(Node::NodeInteger::make("ITEM_DATA", u"物品附加值", -1, max));
            if (HEDLEY_LIKELY(!descriptions.has_value())) {
                node = std::move(nodeAllData);
            } else {
                nodeChildren.push_back(nodeAllData);
                std::vector<const Node::NodeBase *> nodeDataChildren;
                nodeDataChildren.reserve(descriptions.value().size());
                size_t i = 0;
                for (const auto &item: descriptions.value()) {
                    auto nodeChild = std::make_shared<Node::NodeText>(
                            "ITEM_PER_DATA", item,
                            NormalId::make(utf8::utf8to16(std::to_string(i++)), item),
                            [](const Node::NodeBase *node1, TokenReader &tokenReader) -> ASTNode {
                                return tokenReader.readIntegerASTNode(node1);
                            });
                    nodeDataChildren.push_back(nodeChild.get());
                    nodeChildren.push_back(std::move(nodeChild));
                }
                auto nodeOr = std::make_shared<Node::NodeOr>(
                        std::move(nodeDataChildren), false);
                node = std::make_shared<Node::NodeOr>(
                        std::vector<const Node::NodeBase *>{nodeOr.get(), nodeAllData.get()},
                        false, true);
                nodeChildren.push_back(std::move(nodeOr));
            }
        }
        return node;
    }

}// namespace CHelper