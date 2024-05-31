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
        Profile::push(ColorStringBuilder()
                              .red("linking startNode \"")
                              .purple(startNodeId)
                              .red("\" to nodes")
                              .build());
        for (const auto &item: nodes) {
            item->init(cpack);
        }
        if (HEDLEY_LIKELY(startNodeId != "LF")) {
            for (auto &node: nodes) {
                if (HEDLEY_UNLIKELY(node->id == startNodeId)) {
                    start = node.get();
                    break;
                }
            }
        }
        if (HEDLEY_UNLIKELY(start == nullptr)) {
            throw Exception::UnknownNodeId(startNodeId, id.value());
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
                "JSON_STRING", "JSON字符串");
        static std::unique_ptr<NodeBase> jsonInteger = std::make_unique<NodeJsonInteger>(
                "JSON_INTEGER", "JSON整数", std::nullopt, std::nullopt);
        static std::unique_ptr<NodeBase> jsonFloat = std::make_unique<NodeJsonFloat>(
                "JSON_FLOAT", "JSON小数", std::nullopt, std::nullopt);
        static std::unique_ptr<NodeBase> jsonNull = std::make_unique<NodeJsonNull>(
                "JSON_NULL", "JSON空值");
        static std::unique_ptr<NodeBase> jsonBoolean = std::make_unique<NodeJsonBoolean>(
                "JSON_BOOLEAN", "JSON布尔值", std::nullopt, std::nullopt);
        static std::unique_ptr<NodeBase> jsonList = std::make_unique<NodeJsonList>(
                "JSON_LIST", "JSON列表");
        static std::unique_ptr<NodeBase> jsonObject = std::make_unique<NodeJsonObject>(
                "JSON_OBJECT", "JSON对象");
        static std::unique_ptr<NodeBase> jsonElement = std::make_unique<NodeOr>(
                "JSON_ELEMENT", "JSON元素",
                std::vector<const NodeBase *>{
                        jsonBoolean.get(), jsonFloat.get(),
                        jsonInteger.get(), jsonNull.get(),
                        jsonString.get(), jsonList.get(),
                        jsonObject.get()},
                false, false,
                true, "类型不匹配，当前内容不是有效的JSON元素");
        return jsonElement.get();
    }

    void from_json(const nlohmann::json &j, std::unique_ptr<NodeJsonElement> &t) {
        t = std::make_unique<NodeJsonElement>();
        JsonUtil::decode(j, "id", t->id);
        if (HEDLEY_UNLIKELY(!t->id.has_value())) {
            Profile::push("dismiss json data id");
            throw Exception::NodeLoadFailed();
        }
        Profile::push(ColorStringBuilder().red("loading nodes").build());
        JsonUtil::decode(j, "node", t->nodes);
        Profile::next(ColorStringBuilder().red("loading start nodes").build());
        JsonUtil::decode(j, "start", t->startNodeId);
        Profile::pop();
    }

    void to_json(nlohmann::json &j, const std::unique_ptr<NodeJsonElement> &t) {
        JsonUtil::encode(j, "id", t->id);
        JsonUtil::encode(j, "node", t->nodes);
        JsonUtil::encode(j, "start", t->startNodeId);
    }

    void from_binary(BinaryReader &binaryReader, std::unique_ptr<NodeJsonElement> &t) {
        t = std::make_unique<NodeJsonElement>();
        t->id = std::make_optional<std::string>();
        binaryReader.decode(t->id.value());
        binaryReader.decode(t->nodes);
        if (HEDLEY_UNLIKELY(!t->id.has_value())) {
            Profile::push("dismiss json data id");
            throw Exception::NodeLoadFailed();
        }
        binaryReader.decode(t->startNodeId);
    }

    void to_binary(BinaryWriter &binaryWriter, const std::unique_ptr<NodeJsonElement> &t) {
        binaryWriter.encode(t->id.value());
        binaryWriter.encode(t->nodes);
        binaryWriter.encode(t->startNodeId);
    }

}// namespace CHelper::Node