//
// Created by Yancey on 2024/2/28.
//

#include <chelper/node/NodeBase.h>
#include <chelper/node/json/NodeJsonElement.h>
#include <chelper/node/json/NodeJsonList.h>
#include <chelper/node/util/NodeList.h>
#include <chelper/node/util/NodeSingleSymbol.h>

namespace CHelper::Node {

    static std::unique_ptr<NodeBase> nodLeft = std::make_unique<NodeSingleSymbol>(u'[', u"JSON列表左括号");
    static std::unique_ptr<NodeBase> nodeRight = std::make_unique<NodeSingleSymbol>(u']', u"JSON列表右括号");
    static std::unique_ptr<NodeBase> nodeSeparator = std::make_unique<NodeSingleSymbol>(u',', u"JSON列表分隔符");
    static std::unique_ptr<NodeBase> nodeAllList = std::make_unique<NodeList>(
            nodLeft.get(), NodeJsonElement::getNodeJsonElement(), nodeSeparator.get(), nodeRight.get());

    NodeJsonList::NodeJsonList(const std::optional<std::string> &id,
                               const std::optional<std::u16string> &description,
                               std::string data)
        : NodeSerializable(id, description, false),
          data(std::move(data)) {}

    void NodeJsonList::init(const std::vector<std::unique_ptr<NodeSerializable>> &dataList) {
        for (const auto &item: dataList) {
            if (HEDLEY_UNLIKELY(item->id == data)) {
                nodeList = std::make_unique<NodeList>(nodLeft.get(), item.get(),
                                                      nodeSeparator.get(), nodeRight.get());
                return;
            }
        }
        Profile::push("linking contents to {}", FORMAT_ARG(data));
        Profile::push("failed to find node id -> {}", FORMAT_ARG(data));
        Profile::push("unknown node id -> {} (in node \"{}\")", FORMAT_ARG(id.value_or("UNKNOWN")));
        throw std::runtime_error("unknown node id");
    }

    NodeTypeId::NodeTypeId NodeJsonList::getNodeType() const {
        return NodeTypeId::JSON_LIST;
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

}// namespace CHelper::Node