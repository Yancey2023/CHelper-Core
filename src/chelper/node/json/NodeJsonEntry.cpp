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
            L"JSON_LIST_ELEMENT_SEPARATOR", L"冒号", L':');
    static std::unique_ptr<NodeBase> jsonString = std::make_unique<NodeJsonString>(
            L"JSON_STRING", L"JSON字符串");
    static std::unique_ptr<NodeBase> nodeAllEntry = std::make_unique<NodeEntry>(
            L"JSON_OBJECT_ENTRY", L"JSON对象键值对",
            jsonString.get(), nodeSeparator.get(),
            NodeJsonElement::getNodeJsonElement());

    NodeJsonEntry::NodeJsonEntry(const std::optional<std::wstring> &id,
                                 const std::optional<std::wstring> &description,
                                 std::wstring key,
                                 std::vector<std::wstring> value)
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
                Profile::push(L"linking contents to {}", item);
                Profile::push(L"failed to find node id -> {}", item);
                Profile::push(L"unknown node id -> {} (in node \"{}\")", id.value_or(L"UNKNOWN"), item);
                throw std::runtime_error("unknown node id");
            }
        }
        nodeKey = std::make_unique<NodeText>(
                L"JSON_OBJECT_ENTRY_KEY", L"JSON对象键",
                NormalId::make(L'\"' + key + L'\"', description));
        nodeValue = std::make_unique<NodeOr>(
                L"JSON_OBJECT_ENTRY_VALUE", L"JSON对象值",
                std::move(valueNodes), false);
        nodeEntry = std::make_unique<NodeEntry>(
                L"JSON_OBJECT_ENTRY", L"JSON对象键值对",
                nodeKey.get(), nodeSeparator.get(), nodeValue.get());
    }

    ASTNode NodeJsonEntry::getASTNode(TokenReader &tokenReader, const CPack *cpack) const {
        return getByChildNode(tokenReader, cpack, HEDLEY_UNLIKELY(nodeEntry == nullptr) ? nodeAllEntry.get() : nodeEntry.get());
    }

    NodeBase *NodeJsonEntry::getNodeJsonAllEntry() {
        static NodeJsonEntry nodeJsonAllEntry(L"NODE_JSON_ENTRY", L"JSON对象键值对");
        return &nodeJsonAllEntry;
    }

    CODEC_WITH_UNIQUE_PTR(NodeJsonEntry, key, description, value)

}// namespace CHelper::Node