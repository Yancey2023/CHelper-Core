//
// Created by Yancey on 2023/11/8.
//

#include <chelper/node/CommandNode.h>
#include <chelper/resources/id/ItemId.h>

namespace CHelper {

   const Node::NodeWithType &ItemId::getNode() {
        if (node == nullptr) [[unlikely]] {
            if (max.has_value() && max.value() < 0) [[unlikely]] {
                throw std::runtime_error("item id max data value should be a positive number");
            }
            auto nodeAllData = new Node::NodeInteger("ITEM_DATA", u"物品附加值", -1, max);
            nodeChildren.nodes.emplace_back(*nodeAllData);
            if (!descriptions.has_value()) [[unlikely]] {
                node = std::make_unique<Node::NodeWithType>(*nodeAllData);
            } else {
                std::vector<Node::NodeWithType> nodeDataChildren;
                nodeDataChildren.reserve(descriptions.value().size());
                size_t i = 0;
                for (const auto &item: descriptions.value()) {
                    auto nodeChild = new Node::NodeText(
                            "ITEM_PER_DATA", item,
                            NormalId::make(utf8::utf8to16(std::to_string(i++)), item),
                            [](const Node::NodeWithType &node1, TokenReader &tokenReader) -> ASTNode {
                                return tokenReader.readIntegerASTNode(node1);
                            });
                    nodeDataChildren.emplace_back(*nodeChild);
                    nodeChildren.nodes.emplace_back(*nodeChild);
                }
                nodeDataChildren.emplace_back(*nodeAllData);
                auto nodeOr = new Node::NodeOr(std::move(nodeDataChildren), false);
                nodeChildren.nodes.emplace_back(*nodeOr);
                node = std::make_unique<Node::NodeWithType>(*nodeOr);
            }
        }
        return *node;
    }

}// namespace CHelper