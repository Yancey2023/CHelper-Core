//
// Created by Yancey on 2024/2/28.
//

#include "NodeJsonObject.h"

#include <utility>
#include "../util/NodeSingleSymbol.h"
#include "../util/NodeList.h"
#include "../util/NodeOr.h"
#include "NodeJsonElement.h"

namespace CHelper::Node {

    static std::unique_ptr<NodeBase> nodLeft = std::make_unique<NodeSingleSymbol>(
            "JSON_LIST_LEFT", "JSON列表左括号", '{');
    static std::unique_ptr<NodeBase> nodeRight = std::make_unique<NodeSingleSymbol>(
            "JSON_LIST_RIGHT", "JSON列表右括号", '}');
    static std::unique_ptr<NodeBase> nodeSeparator = std::make_unique<NodeSingleSymbol>(
            "JSON_LIST_SEPARATOR", "JSON列表分隔符", ',');
    static std::unique_ptr<NodeBase> nodeAllEntry = std::make_unique<NodeJsonEntry>(
            "JSON_LIST_ENTRY", "JSON对象键值对", "", "");
    static std::unique_ptr<NodeBase> nodeAllList = std::make_unique<NodeList>(
            "JSON_OBJECT", "JSON对象",
            nodLeft.get(), nodeAllEntry.get(),
            nodeSeparator.get(), nodeRight.get());

    NodeJsonObject::NodeJsonObject(const std::optional<std::string> &id,
                                   const std::optional<std::string> &description,
                                   std::vector<std::unique_ptr<NodeBase>> data)
            : NodeBase(id, description, false),
              data(std::move(data)) {}

    inline std::vector<std::unique_ptr<NodeBase>> getDataFromJson(const nlohmann::json &j) {
        std::vector<std::unique_ptr<NodeBase>> data;
        for (const auto &item: j.at("data")) {
            data.push_back(std::make_unique<NodeJsonEntry>(item));
        }
        return data;
    }

    NodeJsonObject::NodeJsonObject(const nlohmann::json &j,
                                   [[maybe_unused]] const CPack &cpack)
            : NodeBase(j, false),
              data(getDataFromJson(j)) {}

    void NodeJsonObject::init(const std::vector<std::unique_ptr<NodeBase>> &dataList) {
        if (data.empty()) {
            nodeElement = nullptr;
            nodeList = nullptr;
            return;
        }
        std::vector<const NodeBase *> nodeElementData;
        for (const auto &item: data) {
            ((NodeJsonEntry *) item.get())->init(dataList);
            nodeElementData.push_back(item.get());
        }
        nodeElement = std::make_unique<NodeOr>("JSON_OBJECT_ELEMENT", "JSON对象键值对",
                                               nodeElementData, false);
        nodeList = std::make_unique<NodeList>("JSON_OBJECT", "JSON对象",
                                              nodLeft.get(), nodeElement.get(), nodeSeparator.get(), nodeRight.get());
    }

    NodeType *NodeJsonObject::getNodeType() const {
        return NodeType::JSON_OBJECT.get();
    }

    ASTNode NodeJsonObject::getASTNode(TokenReader &tokenReader, const CPack *cpack) const {
        if (nodeList == nullptr) {
            return getByChildNode(tokenReader, cpack, nodeAllList.get(), "node json all object");
        }
        tokenReader.push();
        ASTNode result1 = nodeList->getASTNode(tokenReader, cpack);
        size_t index1 = tokenReader.index;
        tokenReader.restore();
        tokenReader.push();
        ASTNode result2 = getByChildNode(tokenReader, cpack, nodeAllList.get(), "node json all object");
        size_t index2 = tokenReader.index;
        tokenReader.restore();
        tokenReader.push();
        tokenReader.index = result1.isError() ? index2 : index1;
        return ASTNode::orNode(this, {result1, result2}, tokenReader.collect());
    }

    bool NodeJsonObject::collectSuggestions(const ASTNode *astNode,
                                            size_t index,
                                            std::vector<Suggestion> &suggestions) const {
        return astNode->id == "node json all object";
    }

} // CHelper::Node