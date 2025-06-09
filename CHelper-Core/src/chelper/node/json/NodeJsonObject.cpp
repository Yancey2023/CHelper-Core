//
// Created by Yancey on 2024/2/28.
//

#include <chelper/node/json/NodeJsonObject.h>
#include <chelper/node/util/NodeSingleSymbol.h>

namespace CHelper::Node {

    NodeJsonObject::NodeJsonObject(const std::optional<std::string> &id,
                                   const std::optional<std::u16string> &description)
        : NodeSerializable(id, description, false) {
        nodeElement1 = nullptr;
        std::vector<const NodeBase *> nodeElementData;
        nodeElementData.push_back(NodeJsonEntry::getNodeJsonAllEntry());
        nodeElement2 = std::make_unique<NodeOr>(std::move(nodeElementData), false, true);
        static std::unique_ptr<NodeBase> nodLeft = std::make_unique<NodeSingleSymbol>(u'{', u"JSON列表左括号");
        static std::unique_ptr<NodeBase> nodeRight = std::make_unique<NodeSingleSymbol>(u'}', u"JSON列表右括号");
        static std::unique_ptr<NodeBase> nodeSeparator = std::make_unique<NodeSingleSymbol>(u',', u"JSON列表分隔符");
        nodeList = std::make_unique<NodeList>(
                nodLeft.get(), nodeElement2.get(),
                nodeSeparator.get(), nodeRight.get());
    }

    void NodeJsonObject::init(const CPack &cpack) {
        if (HEDLEY_UNLIKELY(data.empty())) {
            nodeElement1 = nullptr;
        } else {
            std::vector<const NodeBase *> nodeElementData;
            nodeElementData.reserve(data.size());
            for (const auto &item: data) {
                nodeElementData.push_back(item.get());
            }
            nodeElement1 = std::make_unique<NodeOr>(std::move(nodeElementData), false);
        }
        std::vector<const NodeBase *> nodeElementData;
        if (HEDLEY_LIKELY(nodeElement1 != nullptr)) {
            nodeElementData.reserve(2);
            nodeElementData.push_back(nodeElement1.get());
        }
        nodeElementData.push_back(NodeJsonEntry::getNodeJsonAllEntry());
        nodeElement2 = std::make_unique<NodeOr>(std::move(nodeElementData), false, true);
        static std::unique_ptr<NodeBase> nodLeft = std::make_unique<NodeSingleSymbol>(u'{', u"JSON列表左括号");
        static std::unique_ptr<NodeBase> nodeRight = std::make_unique<NodeSingleSymbol>(u'}', u"JSON列表右括号");
        static std::unique_ptr<NodeBase> nodeSeparator = std::make_unique<NodeSingleSymbol>(u',', u"JSON列表分隔符");
        nodeList = std::make_unique<NodeList>(
                nodLeft.get(), nodeElement2.get(),
                nodeSeparator.get(), nodeRight.get());
    }

    NodeTypeId::NodeTypeId NodeJsonObject::getNodeType() const {
        return NodeTypeId::JSON_OBJECT;
    }

    ASTNode NodeJsonObject::getASTNode(TokenReader &tokenReader, const CPack *cpack) const {
        return getByChildNode(tokenReader, cpack, nodeList.get());
    }

}// namespace CHelper::Node