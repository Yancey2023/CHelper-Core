//
// Created by Yancey on 2023/11/8.
//

#include "ItemId.h"
#include "../../node/param/NodeText.h"
#include "../../node/util/NodeOr.h"
#include "../../node/param/NodeInteger.h"

namespace CHelper {

    static std::shared_ptr<Node::NodeBase> getNodeData(const std::optional<int> &max,
                                                       const std::optional<std::vector<std::string>> &descriptions) {
        if (descriptions.has_value()) {
            auto childNodes = std::make_shared<std::vector<std::shared_ptr<Node::NodeBase>>>();
            size_t i = 0;
            for (const auto &item: descriptions.value()) {
                childNodes->push_back(std::make_shared<Node::NodeText>(
                        "ITEM_PER_DATA", item,
                        std::make_shared<NormalId>(std::to_string(i++), item)));
            }
            return std::make_shared<Node::NodeOr>("ITEM_DATA", "物品附加值",
                                                  childNodes, false);
        } else {
            return std::make_shared<Node::NodeInteger>
                    ("ITEM_DATA", "物品附加值", -1, max);
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

    void ItemId::toJson(nlohmann::json &j) const {
        NamespaceId::toJson(j);
        FROM_JSON_OPTIONAL(j, max, int);
        FROM_JSON_OPTIONAL(j, descriptions, std::vector<std::string>);
    }

} // CHelper