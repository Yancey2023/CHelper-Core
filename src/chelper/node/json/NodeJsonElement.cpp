//
// Created by Yancey on 2024/2/28.
//

#include "NodeJsonElement.h"
#include "NodeJsonBoolean.h"
#include "NodeJsonFloat.h"
#include "NodeJsonInteger.h"
#include "NodeJsonList.h"
#include "NodeJsonNull.h"
#include "NodeJsonObject.h"
#include "NodeJsonString.h"

namespace CHelper::Node {

    NodeType *NodeJsonElement::getNodeType() const {
        return NodeType::JSON_ELEMENT.get();
    }

    void NodeJsonElement::init(const CPack &cpack) {
        Profile::push(L"linking startNode \"{}\" to nodes", startNodeId);
        for (const auto &item: nodes) {
            item->init(cpack);
        }
        if (HEDLEY_LIKELY(startNodeId != L"LF")) {
            for (auto &node: nodes) {
                if (HEDLEY_UNLIKELY(node->id == startNodeId)) {
                    start = node.get();
                    break;
                }
            }
        }
        if (HEDLEY_UNLIKELY(start == nullptr)) {
            Profile::push(L"unknown node id -> {} (in node \"{}\")", startNodeId);
        }
        for (const auto &item: nodes) {
            if (HEDLEY_UNLIKELY(item->getNodeType() == NodeType::JSON_LIST.get())) {
                ((NodeJsonList *) item.get())->init(nodes);
            } else if (HEDLEY_UNLIKELY(item->getNodeType() == NodeType::JSON_OBJECT.get())) {
                for (const auto &item2: ((NodeJsonObject *) item.get())->data) {
                    ((NodeJsonEntry *) item2.get())->init(nodes);
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
                L"JSON_STRING", L"JSON字符串");
        static std::unique_ptr<NodeBase> jsonInteger = std::make_unique<NodeJsonInteger>(
                L"JSON_INTEGER", L"JSON整数", std::nullopt, std::nullopt);
        static std::unique_ptr<NodeBase> jsonFloat = std::make_unique<NodeJsonFloat>(
                L"JSON_FLOAT", L"JSON小数", std::nullopt, std::nullopt);
        static std::unique_ptr<NodeBase> jsonNull = std::make_unique<NodeJsonNull>(
                L"JSON_NULL", L"JSON空值");
        static std::unique_ptr<NodeBase> jsonBoolean = std::make_unique<NodeJsonBoolean>(
                L"JSON_BOOLEAN", L"JSON布尔值", std::nullopt, std::nullopt);
        static std::unique_ptr<NodeBase> jsonList = std::make_unique<NodeJsonList>(
                L"JSON_LIST", L"JSON列表");
        static std::unique_ptr<NodeBase> jsonObject = std::make_unique<NodeJsonObject>(
                L"JSON_OBJECT", L"JSON对象");
        static std::unique_ptr<NodeBase> jsonElement = std::make_unique<NodeOr>(
                L"JSON_ELEMENT", L"JSON元素",
                std::vector<const NodeBase *>{
                        jsonBoolean.get(), jsonFloat.get(),
                        jsonInteger.get(), jsonNull.get(),
                        jsonString.get(), jsonList.get(),
                        jsonObject.get()},
                false, false,true,
                L"类型不匹配，当前内容不是有效的JSON元素");
        return jsonElement.get();
    }

#if CHelperOnlyReadBinary != true
    void from_json(const nlohmann::json &j, std::unique_ptr<NodeJsonElement> &t) {
        t = std::make_unique<NodeJsonElement>();
        JsonUtil::decode(j, "id", t->id);
        if (HEDLEY_UNLIKELY(!t->id.has_value())) {
            throw std::runtime_error("dismiss json data id");
        }
        Profile::push(L"loading nodes");
        JsonUtil::decode(j, "node", t->nodes);
        Profile::next(L"loading start nodes");
        JsonUtil::decode(j, "start", t->startNodeId);
        Profile::pop();
    }

    void to_json(nlohmann::json &j, const std::unique_ptr<NodeJsonElement> &t) {
        JsonUtil::encode(j, "id", t->id);
        JsonUtil::encode(j, "node", t->nodes);
        JsonUtil::encode(j, "start", t->startNodeId);
    }
#endif

    void from_binary(BinaryReader &binaryReader, std::unique_ptr<NodeJsonElement> &t) {
        t = std::make_unique<NodeJsonElement>();
        t->id = std::make_optional<std::wstring>();
        binaryReader.decode(t->id.value());
        binaryReader.decode(t->nodes);
        if (HEDLEY_UNLIKELY(!t->id.has_value())) {
            throw std::runtime_error("dismiss json data id");
        }
        binaryReader.decode(t->startNodeId);
    }

#if CHelperOnlyReadBinary != true
    void to_binary(BinaryWriter &binaryWriter, const std::unique_ptr<NodeJsonElement> &t) {
        binaryWriter.encode(t->id.value());
        binaryWriter.encode(t->nodes);
        binaryWriter.encode(t->startNodeId);
    }
#endif

}// namespace CHelper::Node