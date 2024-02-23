//
// Created by Yancey on 2023/11/11.
//

#include "NodeItem.h"
#include "NodeNamespaceId.h"
#include "NodeInteger.h"
#include "NodeString.h"
#include "../item/NodeItemId.h"

namespace CHelper::Node {

    const static std::shared_ptr<NodeInteger> nodeCount = std::make_shared<NodeInteger>
            ("ITEM_COUNT", "物品数量", 0, std::nullopt);
    const static std::shared_ptr<NodeInteger> nodeData = std::make_shared<NodeInteger>
            ("ITEM_DATA", "物品附加值", -1, std::nullopt);
    //TODO 物品组件解析
    const static std::shared_ptr<NodeString> nodeComponent = std::make_shared<NodeString>
            ("ITEM_COMPONENT", "物品组件", true, true);

    NODE_TYPE("ITEM", NodeItem)

    NodeItem::NodeItem(const std::optional<std::string> &id,
                       const std::optional<std::string> &description,
                       const CHelper::Node::NodeItemType::NodeItemType nodeItemType,
                       const std::vector<std::shared_ptr<CHelper::NamespaceId>> *contents)
            : NodeBase(id, description),
              nodeItemType(nodeItemType) {}

    NodeItem::NodeItem(const nlohmann::json &j,
                       const CPack &cpack) :
            NodeBase(j, cpack),
            nodeItemType(FROM_JSON(j, nodeItemType, CHelper::Node::NodeItemType::NodeItemType)) {}

    void NodeItem::toJson(nlohmann::json &j) const {
        NodeBase::toJson(j);
        TO_JSON(j, nodeItemType);
    }

    ASTNode NodeItem::getASTNode(TokenReader &tokenReader, const CPack &cpack) const {
        switch (nodeItemType) {
            case NodeItemType::ITEM_GIVE:
                return getOptionalASTNode(tokenReader, cpack, false,
                                          {NodeItemId::getInstance(), nodeCount, nodeData, nodeComponent});
            case NodeItemType::ITEM_CLEAR:
                return getOptionalASTNode(tokenReader, cpack, false,
                                          {NodeItemId::getInstance(), nodeData, nodeCount});
            default:
                return getOptionalASTNode(tokenReader, cpack, false,
                                          {NodeItemId::getInstance(), nodeCount, nodeData, nodeComponent});
        }
    }

    std::optional<std::string> NodeItem::getDescription(const ASTNode *node, size_t index) const {
        return std::nullopt;
    }

} // CHelper::Node