//
// Created by Yancey on 2024/2/28.
//

#include "NodeJsonList.h"
#include "../util/NodeSingleSymbol.h"
#include "../util/NodeList.h"

namespace CHelper::Node {

    static std::shared_ptr<NodeBase> nodLeft = std::make_shared<NodeSingleSymbol>(
            "JSON_LIST_LEFT", "JSON列表左括号", '[');
    static std::shared_ptr<NodeBase> nodeRight = std::make_shared<NodeSingleSymbol>(
            "JSON_LIST_RIGHT", "JSON列表右括号", ']');
    static std::shared_ptr<NodeBase> nodeSeparator = std::make_shared<NodeSingleSymbol>(
            "JSON_LIST_SEPARATOR", "JSON列表分隔符", ',');

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
        return getByChildNode(tokenReader, cpack, nodeList);
    }

} // CHelper::Node