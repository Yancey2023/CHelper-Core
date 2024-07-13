//
// Created by Yancey on 2024/2/28.
//

#include "NodeJsonEntry.h"

#include "../util/NodeEntry.h"
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
                                 std::string value)
        : NodeBase(id, description, false),
          key(std::move(key)),
          value(std::move(value)) {}

    NodeType *NodeJsonEntry::getNodeType() const {
        return NodeType::JSON_ENTRY.get();
    }

    void NodeJsonEntry::init(const std::vector<std::unique_ptr<NodeBase>> &dataList) {
        for (const auto &item: dataList) {
            if (HEDLEY_UNLIKELY(item->id == value)) {
                nodeKey = std::make_unique<NodeText>(
                        "JSON_OBJECT_ENTRY_KEY", "JSON对象键",
                        NormalId::make('\"' + key + '\"', description));
                nodeEntry = std::make_unique<NodeEntry>(
                        "JSON_OBJECT_ENTRY", "JSON对象键值对",
                        nodeKey.get(), nodeSeparator.get(), item.get());
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
        throw std::runtime_error(ColorStringBuilder()
                                         .red("unknown node id")
                                         .normal(" -> ")
                                         .purple(id.value_or("UNKNOWN"))
                                         .red(" (in node \"")
                                         .purple(value)
                                         .red("\")")
                                         .build());
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