//
// Created by Yancey on 2023/11/8.
//

#include "ItemId.h"
#include "../../node/param/NodeText.h"
#include "../../node/util/NodeOr.h"
#include "../../node/param/NodeInteger.h"

namespace CHelper {

    static Node::NodeBase *
    getNodeData(const std::optional<int> &max,
                const std::optional<std::vector<std::string>> &descriptions) {
        auto *nodeAllData = new Node::NodeInteger("ITEM_DATA", "物品附加值", -1, max);
        if (!descriptions.has_value()) {
            return nodeAllData;
        }
        std::vector<const Node::NodeBase *> nodeDataChildren;
        nodeDataChildren.reserve(descriptions.value().size());
        size_t i = 0;
        for (const auto &item: descriptions.value()) {
            nodeDataChildren.push_back(new Node::NodeText(
                    "ITEM_PER_DATA", item,
                    std::make_shared<NormalId>(std::to_string(i++), item),
                    [](const Node::NodeBase *node, TokenReader &tokenReader) -> ASTNode {
                        return tokenReader.readIntegerASTNode(node);
                    }));
        }
        auto *nodeOr = new Node::NodeOr("ITEM_DATA", "物品附加值", std::move(nodeDataChildren), false);
        return new Node::NodeOr("ITEM_DATA", "物品附加值", {nodeOr, nodeAllData}, false, true);
    }

    ItemId::ItemId(const std::optional<std::string> &nameSpace,
                   const std::string &name,
                   const std::optional<std::string> &description,
                   const std::optional<int> &max,
                   const std::optional<std::vector<std::string>> &descriptions)
            : NamespaceId(nameSpace, name, description),
              max(max),
              descriptions(descriptions),
              nodeData(getNodeData(max, descriptions)) {}

    ItemId::ItemId(const nlohmann::json &j)
            : NamespaceId(j),
              max(JsonUtil::fromJsonOptional<int>(j, "max")),
              descriptions(JsonUtil::fromJsonOptional<std::vector<std::string>>(j, "descriptions")),
              nodeData(getNodeData(max, descriptions)) {}

    ItemId::~ItemId() {
        if (descriptions.has_value()) {
            auto nodeOr = ((Node::NodeOr *) ((Node::NodeOr *) nodeData)->childNodes[0]);
            for (const auto &item: nodeOr->childNodes) {
                delete item;
            }
            delete nodeOr;
        }
        delete nodeData;
    }

    void ItemId::toJson(nlohmann::json &j) const {
        NamespaceId::toJson(j);
        JsonUtil::toJsonOptional(j, "max", max);
        JsonUtil::toJsonOptional(j, "descriptions", descriptions);
    }

} // CHelper