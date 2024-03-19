//
// Created by Yancey on 2023/11/11.
//

#include "NodeItem.h"
#include "NodeNamespaceId.h"
#include "NodeInteger.h"
#include "NodeString.h"
#include "NodeJson.h"
#include "../../util/TokenUtil.h"

namespace CHelper::Node {

    static std::shared_ptr<NodeBase> nodeCount = std::make_shared<NodeInteger>
            ("ITEM_COUNT", "物品数量", 0, std::nullopt);
    static std::shared_ptr<NodeBase> nodeAllData = std::make_shared<NodeInteger>
            ("ITEM_DATA", "物品附加值", -1, std::nullopt);

    NodeItem::NodeItem(const std::optional<std::string> &id,
                       const std::optional<std::string> &description,
                       const CHelper::Node::NodeItemType::NodeItemType nodeItemType,
                       const std::shared_ptr<std::vector<std::shared_ptr<NamespaceId>>> &contents,
                       const std::shared_ptr<NodeBase> &nodeComponent)
            : NodeBase(id, description, false),
              nodeItemType(nodeItemType),
              itemIds(contents),
              nodeItemId(getNodeItemId(contents)),
              nodeComponent(nodeComponent) {}

    NodeItem::NodeItem(const nlohmann::json &j,
                       const CPack &cpack) :
            NodeBase(j, true),
            nodeItemType(JsonUtil::fromJson<CHelper::Node::NodeItemType::NodeItemType>(j, "nodeItemType")),
            itemIds(cpack.itemIds),
            nodeItemId(getNodeItemId(cpack.itemIds)),
            nodeComponent(std::make_shared<NodeJson>("ITEM_COMPONENT", "物品组件", cpack, "components")) {}

    NodeType *NodeItem::getNodeType() const {
        return NodeType::ITEM.get();
    }

    void NodeItem::toJson(nlohmann::json &j) const {
        NodeBase::toJson(j);
        JsonUtil::toJson(j, "nodeItemType", nodeItemType);
    }

    ASTNode NodeItem::getASTNode(TokenReader &tokenReader, const CPack *cpack) const {
        tokenReader.push();
        ASTNode itemId = nodeItemId.getASTNode(tokenReader, cpack);
        size_t strHash = std::hash<std::string>{}(TokenUtil::toString(itemId.tokens));
        std::shared_ptr<NamespaceId> currentItem = nullptr;
        for (const auto &item: *itemIds) {
            if (HEDLEY_UNLIKELY(item->fastMatch(strHash) || item->idWithNamespace->fastMatch(strHash))) {
                currentItem = item;
                break;
            }
        }
        std::vector<ASTNode> childNodes = {std::move(itemId)};
        const NodeBase *nodeData = currentItem == nullptr ? nodeAllData.get() :
                                   std::static_pointer_cast<ItemId>(currentItem)->nodeData;
        switch (nodeItemType) {
            case NodeItemType::ITEM_GIVE:
                childNodes.push_back(getOptionalASTNode(tokenReader, cpack, false,
                                                        {nodeCount.get(), nodeData, nodeComponent.get()}));
                break;
            case NodeItemType::ITEM_CLEAR:
                childNodes.push_back(getOptionalASTNode(tokenReader, cpack, false,
                                                        {nodeData, nodeCount.get()}));
                break;
            default:
                childNodes.push_back(getOptionalASTNode(tokenReader, cpack, false,
                                                        {nodeCount.get(), nodeData, nodeComponent.get()}));
                break;
        }
        return ASTNode::andNode(this, std::move(childNodes), tokenReader.collect());
    }

    std::optional<std::string> NodeItem::collectDescription(const ASTNode *node, size_t index) const {
        return std::nullopt;
    }

    void NodeItem::collectStructure(const ASTNode *astNode, StructureBuilder &structure, bool isMustHave) const {
        switch (nodeItemType) {
            case NodeItemType::ITEM_GIVE:
                nodeItemId.collectStructure(nullptr, structure, isMustHave);
                nodeCount->collectStructure(nullptr, structure, false);
                nodeAllData->collectStructure(nullptr, structure, false);
                nodeComponent->collectStructure(nullptr, structure, false);
                break;
            case NodeItemType::ITEM_CLEAR:
                nodeItemId.collectStructure(nullptr, structure, isMustHave);
                nodeAllData->collectStructure(nullptr, structure, false);
                nodeCount->collectStructure(nullptr, structure, false);
                break;
            default:
                nodeItemId.collectStructure(nullptr, structure, isMustHave);
                nodeCount->collectStructure(nullptr, structure, false);
                nodeAllData->collectStructure(nullptr, structure, false);
                nodeComponent->collectStructure(nullptr, structure, false);
                break;
        }
    }

    NodeNamespaceId
    NodeItem::getNodeItemId(const std::shared_ptr<std::vector<std::shared_ptr<NamespaceId>>> &contents) {
        return {"ITEM_ID", "物品ID", "items", true, contents};
    }

} // CHelper::Node