//
// Created by Yancey on 2024/2/28.
//

#include <chelper/node/json/NodeJsonElement.h>
#include <chelper/node/json/NodeJsonEntry.h>
#include <chelper/node/json/NodeJsonList.h>
#include <chelper/node/json/NodeJsonNull.h>
#include <chelper/node/json/NodeJsonObject.h>
#include <chelper/node/json/NodeJsonString.h>
#include <chelper/node/template/NodeTemplateBoolean.h>
#include <chelper/node/template/NodeTemplateNumber.h>

namespace CHelper::Node {

    NodeTypeId::NodeTypeId NodeJsonElement::getNodeType() const {
        return NodeTypeId::JSON_ELEMENT;
    }

    void NodeJsonElement::init(const CPack &cpack) {
        Profile::push("linking startNode \"{}\" to nodes", startNodeId);
        for (const auto &item: nodes) {
            item->init(cpack);
        }
        if (HEDLEY_LIKELY(startNodeId != u"LF")) {
            for (auto &node: nodes) {
                if (HEDLEY_UNLIKELY(node->id == startNodeId)) {
                    start = node.get();
                    break;
                }
            }
        }
        if (HEDLEY_UNLIKELY(start == nullptr)) {
            Profile::push("unknown node id -> {} (in node \"{}\")", startNodeId);
        }
        for (const auto &item: nodes) {
            if (HEDLEY_UNLIKELY(item->getNodeType() == NodeTypeId::JSON_LIST)) {
                ((NodeJsonList *) item.get())->init(nodes);
            } else if (HEDLEY_UNLIKELY(item->getNodeType() == NodeTypeId::JSON_OBJECT)) {
                for (const auto &item2: reinterpret_cast<NodeJsonObject *>(item.get())->data) {
                    item2.get()->init(nodes);
                }
            }
        }
        Profile::pop();
    }

    ASTNode NodeJsonElement::getASTNode(TokenReader &tokenReader, const CPack *cpack) const {
        return getByChildNode(tokenReader, cpack, start);
    }

    NodeBase *NodeJsonElement::getNodeJsonElement() {
        static std::unique_ptr<NodeBase> jsonString = std::make_unique<NodeJsonString>(
                u"JSON_STRING", u"JSON字符串");
        static std::unique_ptr<NodeBase> jsonInteger = NodeJsonInteger::make(
                u"JSON_INTEGER", u"JSON整数", std::nullopt, std::nullopt);
        static std::unique_ptr<NodeBase> jsonFloat = NodeJsonFloat::make(
                u"JSON_FLOAT", u"JSON小数", std::nullopt, std::nullopt);
        static std::unique_ptr<NodeBase> jsonNull = std::make_unique<NodeJsonNull>(
                u"JSON_NULu", u"JSON空值");
        static std::unique_ptr<NodeBase> jsonBoolean = NodeJsonBoolean::make(
                u"JSON_BOOLEAN", u"JSON布尔值", std::nullopt, std::nullopt);
        static std::unique_ptr<NodeBase> jsonList = std::make_unique<NodeJsonList>(
                u"JSON_LIST", u"JSON列表");
        static std::unique_ptr<NodeBase> jsonObject = std::make_unique<NodeJsonObject>(
                u"JSON_OBJECT", u"JSON对象");
        static std::unique_ptr<NodeBase> jsonElement = std::make_unique<NodeOr>(
                u"JSON_ELEMENT", u"JSON元素",
                std::vector<const NodeBase *>{
                        jsonBoolean.get(), jsonFloat.get(),
                        jsonInteger.get(), jsonNull.get(),
                        jsonString.get(), jsonList.get(),
                        jsonObject.get()},
                false, false, true,
                u"类型不匹配，当前内容不是有效的JSON元素");
        return jsonElement.get();
    }

}// namespace CHelper::Node