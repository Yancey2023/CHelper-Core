//
// Created by Yancey on 2023/11/11.
//

#include "NodeItem.h"
#include "NodeInteger.h"
#include "NodeJson.h"
#include "NodeNamespaceId.h"
#include "NodeString.h"

namespace CHelper::Node {

    static std::shared_ptr<NodeBase> nodeCount = std::make_shared<NodeInteger>("ITEM_COUNT", "物品数量", 0, std::nullopt);
    static std::shared_ptr<NodeBase> nodeAllData = std::make_shared<NodeInteger>("ITEM_DATA", "物品附加值", -1, std::nullopt);

    void NodeItem::init(const CPack &cpack) {
        itemIds = cpack.itemIds;
        nodeItemId = std::make_unique<NodeNamespaceId>("ITEM_ID", "物品ID", "items", true);
        nodeComponent = std::make_unique<NodeJson>("ITEM_COMPONENT", "物品组件", "components");
        nodeItemId->init(cpack);
        nodeComponent->init(cpack);
    }

    NodeType *NodeItem::getNodeType() const {
        return NodeType::ITEM.get();
    }

    ASTNode NodeItem::getASTNode(TokenReader &tokenReader, const CPack *cpack) const {
        tokenReader.push();
        ASTNode itemId = nodeItemId->getASTNode(tokenReader, cpack);
        size_t strHash = std::hash<std::string_view>{}(itemId.tokens.toString());
        std::shared_ptr<NamespaceId> currentItem = nullptr;
        for (const auto &item: *itemIds) {
            if (HEDLEY_UNLIKELY(item->fastMatch(strHash) || item->getIdWithNamespace()->fastMatch(strHash))) {
                currentItem = item;
                break;
            }
        }
        std::vector<ASTNode> childNodes = {std::move(itemId)};
        const NodeBase *nodeData = currentItem == nullptr ? nodeAllData.get() : std::static_pointer_cast<ItemId>(currentItem)->getNode().get();
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
                nodeItemId->collectStructure(nullptr, structure, isMustHave);
                nodeCount->collectStructure(nullptr, structure, false);
                nodeAllData->collectStructure(nullptr, structure, false);
                nodeComponent->collectStructure(nullptr, structure, false);
                break;
            case NodeItemType::ITEM_CLEAR:
                nodeItemId->collectStructure(nullptr, structure, isMustHave);
                nodeAllData->collectStructure(nullptr, structure, false);
                nodeCount->collectStructure(nullptr, structure, false);
                break;
            default:
                nodeItemId->collectStructure(nullptr, structure, isMustHave);
                nodeCount->collectStructure(nullptr, structure, false);
                nodeAllData->collectStructure(nullptr, structure, false);
                nodeComponent->collectStructure(nullptr, structure, false);
                break;
        }
    }

    namespace NodeItemType {

        CODEC_ENUM(NodeItemType, uint8_t)

    }// namespace NodeItemType

    CODEC_NODE(NodeItem, nodeItemType)

}// namespace CHelper::Node