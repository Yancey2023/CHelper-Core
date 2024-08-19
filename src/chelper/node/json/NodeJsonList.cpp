//
// Created by Yancey on 2024/2/28.
//

#include "NodeJsonList.h"
#include "../util/NodeList.h"
#include "../util/NodeSingleSymbol.h"
#include "NodeJsonElement.h"

namespace CHelper::Node {

    static std::unique_ptr<NodeBase> nodLeft = std::make_unique<NodeSingleSymbol>(
            L"JSON_LIST_LEFT", L"JSON列表左括号", L'[');
    static std::unique_ptr<NodeBase> nodeRight = std::make_unique<NodeSingleSymbol>(
            L"JSON_LIST_RIGHT", L"JSON列表右括号", L']');
    static std::unique_ptr<NodeBase> nodeSeparator = std::make_unique<NodeSingleSymbol>(
            L"JSON_LIST_SEPARATOR", L"JSON列表分隔符", L',');
    static std::unique_ptr<NodeBase> nodeAllList = std::make_unique<NodeList>(
            L"JSON_OBJECT", L"JSON对象",
            nodLeft.get(), NodeJsonElement::getNodeJsonElement(), nodeSeparator.get(), nodeRight.get());

    NodeJsonList::NodeJsonList(const std::optional<std::wstring> &id,
                               const std::optional<std::wstring> &description,
                               std::wstring data)
        : NodeBase(id, description, false),
          data(std::move(data)) {}

    void NodeJsonList::init(const std::vector<std::unique_ptr<NodeBase>> &dataList) {
        for (const auto &item: dataList) {
            if (HEDLEY_UNLIKELY(item->id == data)) {
                nodeList = std::make_unique<NodeList>(L"JSON_LIST", L"JSON列表",
                                                      nodLeft.get(), item.get(),
                                                      nodeSeparator.get(), nodeRight.get());
                return;
            }
        }
        Profile::push("linking contents to {}", data);
        Profile::push("failed to find node id -> {}", data);
        Profile::push("unknown node id -> {} (in node \"{}\")", id.value_or(L"UNKNOWN"), data);
        throw std::runtime_error("unknown node id");
    }

    NodeType *NodeJsonList::getNodeType() const {
        return NodeType::JSON_LIST.get();
    }

    ASTNode NodeJsonList::getASTNode(TokenReader &tokenReader, const CPack *cpack) const {
        if (HEDLEY_UNLIKELY(nodeList == nullptr)) {
            return getByChildNode(tokenReader, cpack, nodeAllList.get(), ASTNodeId::NODE_JSON_ALL_LIST);
        }
        tokenReader.push();
        ASTNode result1 = nodeList->getASTNode(tokenReader, cpack);
        if (HEDLEY_LIKELY(!result1.isError())) {
            return ASTNode::andNode(this, {std::move(result1)}, tokenReader.collect());
        }
        size_t index1 = tokenReader.index;
        tokenReader.restore();
        tokenReader.push();
        ASTNode result2 = getByChildNode(tokenReader, cpack, nodeAllList.get(), ASTNodeId::NODE_JSON_ALL_LIST);
        size_t index2 = tokenReader.index;
        tokenReader.restore();
        tokenReader.push();
        tokenReader.index = result1.isError() ? index2 : index1;
        return ASTNode::orNode(this, {std::move(result1), std::move(result2)}, tokenReader.collect());
    }

    bool NodeJsonList::collectSuggestions(const ASTNode *astNode,
                                          size_t index,
                                          std::vector<Suggestions> &suggestions) const {
        return astNode->id == ASTNodeId::NODE_JSON_ALL_LIST;
    }

    CODEC_NODE(NodeJsonList, data)

}// namespace CHelper::Node