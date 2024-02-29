//
// Created by Yancey on 2024/2/28.
//

#include "NodeJsonEntry.h"

#include "../util/NodeSingleSymbol.h"
#include "../param/NodeText.h"
#include "../util/NodeEntry.h"

namespace CHelper::Node {

    static std::shared_ptr<NodeBase> nodeSeparator = std::make_shared<NodeSingleSymbol>(
            "JSON_LIST_ELEMENT_SEPARATOR", "冒号", ':');

    NodeJsonEntry::NodeJsonEntry(const std::optional<std::string> &id,
                                 const std::optional<std::string> &description,
                                 std::string key,
                                 std::string value)
            : NodeBase(id, description, false),
              key(std::move(key)),
              value(std::move(value)) {}

    NodeJsonEntry::NodeJsonEntry(const nlohmann::json &j)
            : NodeBase(j, false),
              key(FROM_JSON(j, key, std::string)),
              value(FROM_JSON(j, value, std::string)) {}

    void NodeJsonEntry::init(const std::vector<std::shared_ptr<NodeBase>> &dataList) {
        for (const auto &item: dataList) {
            if (item->id == value) {
                auto keyId = std::make_shared<NormalId>('\"' + key + '\"', description);
                auto nodeKey = std::make_shared<NodeText>("JSON_OBJECT_ENTRY_KEY", "JSON对象键", keyId);
                nodeEntry = std::make_shared<NodeEntry>("JSON_OBJECT_ENTRY", "JSON对象键值对", nodeKey, nodeSeparator,
                                                        item);
                return;
            }
        }
        Profile::push(ColorStringBuilder()
                              .red("linking contents to ")
                              .purple(value)
                              .build());
        Profile::push(ColorStringBuilder()
                              .red("failed to find node id")
                              .normal(" -> ")
                              .purple(value)
                              .build());
        throw Exception::UnknownNodeId(value, id.value_or("UNKNOWN"));
    }

    ASTNode NodeJsonEntry::getASTNode(TokenReader &tokenReader, const CPack &cpack) const {
        return getByChildNode(tokenReader, cpack, nodeEntry);
    }

} // CHelper::Node