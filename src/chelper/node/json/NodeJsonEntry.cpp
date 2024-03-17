//
// Created by Yancey on 2024/2/28.
//

#include "NodeJsonEntry.h"

#include "../util/NodeSingleSymbol.h"
#include "../util/NodeEntry.h"
#include "NodeJsonString.h"
#include "NodeJsonElement.h"

namespace CHelper::Node {

    static std::unique_ptr<NodeBase> nodeSeparator = std::make_unique<NodeSingleSymbol>(
            "JSON_LIST_ELEMENT_SEPARATOR", "冒号", ':');
    static std::unique_ptr<NodeBase> jsonString = std::make_unique<NodeJsonString>(
            "JSON_STRING", "JSON字符串", std::vector<std::unique_ptr<NodeBase>>{});
    static std::unique_ptr<NodeBase> nodeAllEntry = std::make_unique<NodeEntry>(
            "JSON_OBJECT_ENTRY", "JSON对象键值对",
            jsonString.get(), nodeSeparator.get(),
            NodeJsonElement::getNodeJsonElement());

    NodeJsonEntry::NodeJsonEntry(const std::optional<std::string> &id,
                                 const std::optional<std::string> &description,
                                 std::string key,
                                 std::string value)
            : NodeBase(id, description, false),
              key(std::move(key)),
              value(std::move(value)) {}

    NodeJsonEntry::NodeJsonEntry(const nlohmann::json &j)
            : NodeBase(j, false),
              key(JsonUtil::fromJson<std::string>(j, "key")),
              value(JsonUtil::fromJson<std::string>(j, "value")) {}

    void NodeJsonEntry::init(const std::vector<std::unique_ptr<NodeBase>> &dataList) {
        for (const auto &item: dataList) {
            if (item->id == value) {
                nodeKey = std::make_unique<NodeText>("JSON_OBJECT_ENTRY_KEY", "JSON对象键",
                                                     std::make_shared<NormalId>('\"' + key + '\"', description));
                nodeEntry = std::make_unique<NodeEntry>("JSON_OBJECT_ENTRY", "JSON对象键值对", nodeKey.get(),
                                                        nodeSeparator.get(), item.get());
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

    void NodeJsonEntry::toJson(nlohmann::json &j) const {
        JsonUtil::toJson(j, "key", key);
        JsonUtil::toJsonOptionalLikely(j, "description", description);
        JsonUtil::toJson(j, "value", value);
    }

    ASTNode NodeJsonEntry::getASTNode(TokenReader &tokenReader, const CPack *cpack) const {
        if (nodeEntry == nullptr) {
            return getByChildNode(tokenReader, cpack, nodeAllEntry.get(), "node json all entry");
        } else {
            return getByChildNode(tokenReader, cpack, nodeEntry.get());
        }
    }

    bool NodeJsonEntry::collectSuggestions(const ASTNode *astNode,
                                           size_t index,
                                           std::vector<Suggestions> &suggestions) const {
        return astNode->id == "node json all entry";
    }

    NodeBase *NodeJsonEntry::getNodeJsonAllEntry() {
        static NodeJsonEntry nodeJsonAllEntry("NODE_JSON_ENTRY", "JSON对象键值对");
        return &nodeJsonAllEntry;
    }

} // CHelper::Node