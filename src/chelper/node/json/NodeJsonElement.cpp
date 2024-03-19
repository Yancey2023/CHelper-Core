//
// Created by Yancey on 2024/2/28.
//

#include "NodeJsonElement.h"
#include "NodeJsonObject.h"
#include "NodeJsonList.h"
#include "NodeJsonString.h"
#include "NodeJsonInteger.h"
#include "NodeJsonFloat.h"
#include "NodeJsonNull.h"
#include "NodeJsonBoolean.h"

namespace CHelper::Node {

    NodeJsonElement::NodeJsonElement(const std::optional<std::string> &id,
                                     const std::optional<std::string> &description,
                                     std::vector<std::unique_ptr<NodeBase>> nodes,
                                     NodeBase *start)
            : NodeBase(id, description, false),
              nodes(std::move(nodes)),
              start(start) {}

    NodeJsonElement::NodeJsonElement(const nlohmann::json &j,
                                     [[maybe_unused]] const CPack &cpack)
            : NodeBase(j, false) {
        if (HEDLEY_UNLIKELY(!id.has_value())) {
            Profile::push("dismiss json data id");
            throw Exception::NodeLoadFailed();
        }
        Profile::push(ColorStringBuilder().red("loading nodes").build());
        for (const auto &item: j.at("node")) {
            nodes.push_back(getNodeFromJson(item, cpack));
        }
        Profile::next(ColorStringBuilder().red("loading start nodes").build());
        auto startNodeId = JsonUtil::fromJson<std::string>(j, "start");
        Profile::next(ColorStringBuilder()
                              .red("linking startNode \"")
                              .purple(startNodeId)
                              .red("\" to nodes")
                              .build());
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

    void NodeJsonElement::toJson(nlohmann::json &j) const {
        JsonUtil::toJson(j, "id", id.value());
        JsonUtil::toJson(j, "node", nodes);
        JsonUtil::toJson(j, "start", start->id.value());
    }

    ASTNode NodeJsonElement::getASTNode(TokenReader &tokenReader, const CPack *cpack) const {
        return getByChildNode(tokenReader, cpack, start);
    }

    NodeBase *NodeJsonElement::getNodeJsonElement() {
        static std::unique_ptr<NodeBase> jsonString = std::make_unique<NodeJsonString>(
                "JSON_STRING", "JSON字符串", std::vector<std::unique_ptr<NodeBase>>{});
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
                "JSON_ELEMENT", "JSON元素", std::vector<const NodeBase *>{
                        jsonBoolean.get(), jsonFloat.get(), jsonInteger.get(), jsonNull.get(),
                        jsonString.get(), jsonList.get(), jsonObject.get()}, false, false,
                true, "类型不匹配，当前内容不是有效的JSON元素");
        return jsonElement.get();
    }

} // CHelper::Node