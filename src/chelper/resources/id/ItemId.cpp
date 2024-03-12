//
// Created by Yancey on 2023/11/8.
//

#include "ItemId.h"
#include "../../node/param/NodeText.h"
#include "../../node/util/NodeOr.h"
#include "../../node/param/NodeInteger.h"

namespace CHelper {

    static std::vector<const Node::NodeBase *>
    getNodeDataChildren(const std::optional<std::vector<std::string>> &descriptions) {
        std::vector<const Node::NodeBase *> result;

        return result;
    }

    static Node::NodeBase *
    getNodeData(const std::optional<int> &max,
                const std::optional<std::vector<std::string>> &descriptions) {
        if (descriptions.has_value()) {
            std::vector<const Node::NodeBase *> nodeDataChildren;
            size_t i = 0;
            for (const auto &item: descriptions.value()) {
                nodeDataChildren.push_back(new Node::NodeText(
                        "ITEM_PER_DATA", item,
                        std::make_shared<NormalId>(std::to_string(i++), item)));
            }
            return new Node::NodeOr("ITEM_DATA", "物品附加值", nodeDataChildren, false);
        } else {
            return new Node::NodeInteger("ITEM_DATA", "物品附加值", -1, max);
        }
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
              max(FROM_JSON_OPTIONAL(j, max, int)),
              descriptions(FROM_JSON_OPTIONAL(j, descriptions, std::vector<std::string>)),
              nodeData(getNodeData(max, descriptions)) {}

    ItemId::~ItemId() {
        if (descriptions.has_value()) {
            for (const auto &item: ((Node::NodeOr *) nodeData)->childNodes) {
                delete item;
            }
        }
        delete nodeData;
    }

    void ItemId::toJson(nlohmann::json &j) const {
        NamespaceId::toJson(j);
        FROM_JSON_OPTIONAL(j, max, int);
        FROM_JSON_OPTIONAL(j, descriptions, std::vector<std::string>);
    }

} // CHelper