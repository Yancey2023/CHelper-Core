//
// Created by Yancey on 2024/2/28.
//

#include "NodeJsonList.h"
#include "../util/NodeSingleSymbol.h"
#include "../util/NodeList.h"
#include "NodeJsonElement.h"

namespace CHelper::Node {

    static std::shared_ptr<NodeBase> nodLeft = std::make_shared<NodeSingleSymbol>(
            "JSON_LIST_LEFT", "JSON列表左括号", '[');
    static std::shared_ptr<NodeBase> nodeRight = std::make_shared<NodeSingleSymbol>(
            "JSON_LIST_RIGHT", "JSON列表右括号", ']');
    static std::shared_ptr<NodeBase> nodeSeparator = std::make_shared<NodeSingleSymbol>(
            "JSON_LIST_SEPARATOR", "JSON列表分隔符", ',');
    static std::shared_ptr<NodeBase> nodeAllList = std::make_shared<NodeList>(
            "JSON_OBJECT", "JSON对象",
            nodLeft, NodeJsonElement::getNodeJsonElement(), nodeSeparator, nodeRight);

    NodeJsonList::NodeJsonList(const std::optional<std::string> &id,
                               const std::optional<std::string> &description,
                               std::string data)
            : NodeBase(id, description, false),
              data(std::move(data)) {}

    NodeJsonList::NodeJsonList(const nlohmann::json &j,
                               [[maybe_unused]] const CPack &cpack)
            : NodeBase(j, false),
              data(FROM_JSON(j, data, std::string)) {}

    void NodeJsonList::init(const std::vector<std::shared_ptr<NodeBase>> &dataList) {
        for (const auto &item: dataList) {
            if (item->id == data) {
                nodeList = std::make_shared<NodeList>("JSON_LIST", "JSON列表",
                                                      nodLeft, item, nodeSeparator, nodeRight);
                return;
            }
        }
        Profile::push(ColorStringBuilder()
                              .red("linking contents to ")
                              .purple(data)
                              .build());
        Profile::push(ColorStringBuilder()
                              .red("failed to find node id")
                              .normal(" -> ")
                              .purple(data)
                              .build());
        throw Exception::UnknownNodeId(data, id.value_or("UNKNOWN"));
    }

    std::shared_ptr<NodeType> NodeJsonList::getNodeType() const {
        return NodeType::JSON_LIST;
    }

    ASTNode NodeJsonList::getASTNode(TokenReader &tokenReader, const CPack &cpack) const {
        if (nodeList == nullptr) {
            return getByChildNode(tokenReader, cpack, nodeAllList, "node json all list");
        }
        tokenReader.push();
        ASTNode result1 = nodeList->getASTNode(tokenReader, cpack);
        size_t index1 = tokenReader.index;
        tokenReader.restore();
        tokenReader.push();
        ASTNode result2 = getByChildNode(tokenReader, cpack, nodeAllList, "node json all list");
        size_t index2 = tokenReader.index;
        tokenReader.restore();
        tokenReader.push();
        tokenReader.index = result1.isError() ? index2 : index1;
        return ASTNode::orNode(this, {result1, result2}, tokenReader.collect());
    }

    bool NodeJsonList::collectSuggestions(const ASTNode *astNode,
                                          size_t index,
                                          std::vector<Suggestion> &suggestions) const {
        return astNode->id == "node json all list";
    }

} // CHelper::Node