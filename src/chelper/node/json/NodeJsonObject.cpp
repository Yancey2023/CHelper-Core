//
// Created by Yancey on 2024/2/28.
//

#include "NodeJsonObject.h"
#include "../util/NodeSingleSymbol.h"
#include "../util/NodeList.h"
#include "../util/NodeOr.h"
#include "NodeJsonElement.h"

namespace CHelper::Node {

    static std::shared_ptr<NodeBase> nodLeft = std::make_shared<NodeSingleSymbol>(
            "JSON_LIST_LEFT", "JSON列表左括号", '{');
    static std::shared_ptr<NodeBase> nodeRight = std::make_shared<NodeSingleSymbol>(
            "JSON_LIST_RIGHT", "JSON列表右括号", '}');
    static std::shared_ptr<NodeBase> nodeSeparator = std::make_shared<NodeSingleSymbol>(
            "JSON_LIST_SEPARATOR", "JSON列表分隔符", ',');
    static std::shared_ptr<NodeBase> nodeAllEntry = std::make_shared<NodeJsonEntry>(
            "JSON_LIST_ENTRY", "JSON对象键值对", "", "");
    static std::shared_ptr<NodeBase> nodeAllList = std::make_shared<NodeList>(
            "JSON_OBJECT", "JSON对象",
            nodLeft, nodeAllEntry, nodeSeparator, nodeRight);

    NodeJsonObject::NodeJsonObject(const std::optional<std::string> &id,
                                   const std::optional<std::string> &description,
                                   const std::shared_ptr<std::vector<std::shared_ptr<NodeBase>>> &data)
            : NodeBase(id, description, false),
              data(data) {}

    inline std::shared_ptr<std::vector<std::shared_ptr<NodeBase>>> getDataFromJson(const nlohmann::json &j) {
        std::shared_ptr<std::vector<std::shared_ptr<NodeBase>>> data = std::make_shared<std::vector<std::shared_ptr<NodeBase>>>();
        for (const auto &item: j.at("data")) {
            data->push_back(std::make_shared<NodeJsonEntry>(item));
        }
        return data;
    }

    NodeJsonObject::NodeJsonObject(const nlohmann::json &j,
                                   [[maybe_unused]] const CPack &cpack)
            : NodeBase(j, false),
              data(getDataFromJson(j)) {}

    void NodeJsonObject::init(const std::vector<std::shared_ptr<NodeBase>> &dataList) {
        for (const auto &item: *data) {
            std::static_pointer_cast<NodeJsonEntry>(item)->init(dataList);
        }
        auto nodeElement = std::make_shared<NodeOr>("JSON_OBJECT_ELEMENT", "JSON对象键值对", data, false);
        nodeList = std::make_shared<NodeList>("JSON_OBJECT", "JSON对象",
                                              nodLeft, nodeElement, nodeSeparator, nodeRight);
    }

    std::shared_ptr<NodeType> NodeJsonObject::getNodeType() const {
        return NodeType::JSON_OBJECT;
    }

    ASTNode NodeJsonObject::getASTNode(TokenReader &tokenReader, const CPack &cpack) const {
        if (nodeList == nullptr) {
            return getByChildNode(tokenReader, cpack, nodeAllList, "node json all object");
        }
        tokenReader.push();
        ASTNode result1 = nodeList->getASTNode(tokenReader, cpack);
        size_t index1 = tokenReader.index;
        tokenReader.restore();
        tokenReader.push();
        ASTNode result2 = getByChildNode(tokenReader, cpack, nodeAllList, "node json all object");
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