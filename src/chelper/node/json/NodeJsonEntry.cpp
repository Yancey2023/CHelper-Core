//
// Created by Yancey on 2024/2/28.
//

#include "NodeJsonEntry.h"

#include "../util/NodeEntry.h"
#include "../util/NodeOr.h"
#include "../util/NodeSingleSymbol.h"
#include "NodeJsonElement.h"
#include "NodeJsonString.h"

namespace CHelper::Node {

    static std::unique_ptr<NodeBase> nodeSeparator = std::make_unique<NodeSingleSymbol>(
            "JSON_LIST_ELEMENT_SEPARATOR", "冒号", ':');
    static std::unique_ptr<NodeBase> jsonString = std::make_unique<NodeJsonString>(
            "JSON_STRING", "JSON字符串");
    static std::unique_ptr<NodeBase> nodeAllEntry = std::make_unique<NodeEntry>(
            "JSON_OBJECT_ENTRY", "JSON对象键值对",
            jsonString.get(), nodeSeparator.get(),
            NodeJsonElement::getNodeJsonElement());

    NodeJsonEntry::NodeJsonEntry(const std::optional<std::string> &id,
                                 const std::optional<std::string> &description,
                                 std::string key,
                                 std::vector<std::string> value)
        : NodeBase(id, description, false),
          key(std::move(key)),
          value(std::move(value)) {}

    NodeType *NodeJsonEntry::getNodeType() const {
        return NodeType::JSON_ENTRY.get();
    }

    void NodeJsonEntry::init(const std::vector<std::unique_ptr<NodeBase>> &dataList) {
        std::vector<const NodeBase *> valueNodes;
        for (const auto &item: value) {
            bool notFind = true;
            for (const auto &item2: dataList) {
                if (HEDLEY_UNLIKELY(item2->id == item)) {
                    valueNodes.push_back(item2.get());
                    notFind = false;
                    break;
                }
            }
            if (notFind) {
                Profile::push(ColorStringBuilder()
                                      .red("linking contents to ")
                                      .purple(item)
                                      .build());
                Profile::push(ColorStringBuilder()
                                      .red("failed to find node id")
                                      .normal(" -> ")
                                      .purple(item)
                                      .build());
                throw std::runtime_error(ColorStringBuilder()
                                                 .red("unknown node id")
                                                 .normal(" -> ")
                                                 .purple(id.value_or("UNKNOWN"))
                                                 .red(" (in node \"")
                                                 .purple(item)
                                                 .red("\")")
                                                 .build());
            }
        }
        nodeKey = std::make_unique<NodeText>(
                "JSON_OBJECT_ENTRY_KEY", "JSON对象键",
                NormalId::make('\"' + key + '\"', description));
        nodeValue = std::make_unique<NodeOr>(
                "JSON_OBJECT_ENTRY_VALUE", "JSON对象值",
                std::move(valueNodes), false);
        nodeEntry = std::make_unique<NodeEntry>(
                "JSON_OBJECT_ENTRY", "JSON对象键值对",
                nodeKey.get(), nodeSeparator.get(), nodeValue.get());
    }

    ASTNode NodeJsonEntry::getASTNode(TokenReader &tokenReader, const CPack *cpack) const {
        return getByChildNode(tokenReader, cpack, HEDLEY_UNLIKELY(nodeEntry == nullptr) ? nodeAllEntry.get() : nodeEntry.get());
    }

    NodeBase *NodeJsonEntry::getNodeJsonAllEntry() {
        static NodeJsonEntry nodeJsonAllEntry("NODE_JSON_ENTRY", "JSON对象键值对");
        return &nodeJsonAllEntry;
    }

    CODEC_WITH_UNIQUE_PTR(NodeJsonEntry, key, description, value)

}// namespace CHelper::Node