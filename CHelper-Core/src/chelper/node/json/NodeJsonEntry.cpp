//
// Created by Yancey on 2024/2/28.
//


#include <chelper/node/json/NodeJsonElement.h>
#include <chelper/node/json/NodeJsonEntry.h>
#include <chelper/node/json/NodeJsonString.h>
#include <chelper/node/util/NodeEntry.h>
#include <chelper/node/util/NodeOr.h>
#include <chelper/node/util/NodeSingleSymbol.h>

namespace CHelper::Node {

    static std::unique_ptr<NodeBase> nodeSeparator = std::make_unique<NodeSingleSymbol>(
            "JSON_LIST_ELEMENT_SEPARATOR", u"冒号", u':');
    static std::unique_ptr<NodeBase> jsonString = std::make_unique<NodeJsonString>(
            "JSON_STRING", u"JSON字符串");
    static std::unique_ptr<NodeBase> nodeAllEntry = std::make_unique<NodeEntry>(
            "JSON_OBJECT_ENTRY", u"JSON对象键值对",
            jsonString.get(), nodeSeparator.get(),
            NodeJsonElement::getNodeJsonElement());

    NodeJsonEntry::NodeJsonEntry(const std::optional<std::string> &id,
                                 const std::optional<std::u16string> &description,
                                 std::u16string key,
                                 std::vector<std::string> value)
        : NodeBase(id, description, false),
          key(std::move(key)),
          value(std::move(value)) {}

    NodeTypeId::NodeTypeId NodeJsonEntry::getNodeType() const {
        return NodeTypeId::JSON_ENTRY;
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
                Profile::push("linking contents to {}", FORMAT_ARG(item));
                Profile::push("failed to find node id -> {}", FORMAT_ARG(item));
                Profile::push("unknown node id -> {} (in node \"{}\")", FORMAT_ARG(id.value_or("UNKNOWN")), FORMAT_ARG(item));
                throw std::runtime_error("unknown node id");
            }
        }
        nodeKey = std::make_unique<NodeText>(
                "JSON_OBJECT_ENTRY_KEY", u"JSON对象键",
                NormalId::make(fmt::format(u"\"{}\"", key), description));
        nodeValue = std::make_unique<NodeOr>(
                "JSON_OBJECT_ENTRY_VALUE", u"JSON对象值",
                std::move(valueNodes), false);
        nodeEntry = std::make_unique<NodeEntry>(
                "JSON_OBJECT_ENTRY", u"JSON对象键值对",
                nodeKey.get(), nodeSeparator.get(), nodeValue.get());
    }

    ASTNode NodeJsonEntry::getASTNode(TokenReader &tokenReader, const CPack *cpack, void *private_data) const {
        return getByChildNode(tokenReader, cpack, HEDLEY_UNLIKELY(nodeEntry == nullptr) ? nodeAllEntry.get() : nodeEntry.get());
    }

    NodeBase *NodeJsonEntry::getNodeJsonAllEntry() {
        static NodeJsonEntry nodeJsonAllEntry("NODE_JSON_ENTRY", u"JSON对象键值对");
        return &nodeJsonAllEntry;
    }

}// namespace CHelper::Node