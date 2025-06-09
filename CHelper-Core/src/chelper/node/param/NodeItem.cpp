//
// Created by Yancey on 2023/11/11.
//

#include <chelper/node/param/NodeItem.h>
#include <chelper/node/param/NodeJson.h>
#include <chelper/node/template/NodeTemplateNumber.h>
#include <chelper/resources/CPack.h>

#ifdef CHelperDebug
#include <chelper/node/NodeType.h>
#endif

namespace CHelper::Node {

    static std::shared_ptr<NodeBase> nodeCount = NodeInteger::make("ITEM_COUNT", u"物品数量", 0, std::nullopt);
    static std::shared_ptr<NodeBase> nodeAllData = NodeInteger::make("ITEM_DATA", u"物品附加值", -1, std::nullopt);

    void NodeItem::init(const CPack &cpack) {
        itemIds = cpack.itemIds;
        nodeItemId = std::make_unique<NodeNamespaceId>("ITEM_ID", u"物品ID", "item", true);
        nodeComponent = std::make_unique<NodeJson>("ITEM_COMPONENT", u"物品组件", "components");
        nodeItemId->init(cpack);
        nodeComponent->init(cpack);
    }

    NodeTypeId::NodeTypeId NodeItem::getNodeType() const {
        return NodeTypeId::ITEM;
    }

    ASTNode NodeItem::getASTNode(TokenReader &tokenReader, const CPack *cpack) const {
        tokenReader.push();
        ASTNode itemId = nodeItemId->getASTNode(tokenReader, cpack);
        size_t strHash = std::hash<std::u16string_view>{}(itemId.tokens.toString());
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

    std::optional<std::u16string> NodeItem::collectDescription(const ASTNode *node, size_t index) const {
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

    /**
     * 节点不一定有
     *
     * @param tokenReader token读取器
     * @param cpack 资源包
     * @param isIgnoreChildNodesError true - 第一个错误节点到后面都不算做子节点
     *                                false - 第一个内容为空的错误节点到后面都不算做子节点
     * @param childNodes 子节点
     * @param astNodeId 节点ID
     */
    ASTNode NodeItem::getOptionalASTNode(TokenReader &tokenReader,
                                         const CPack *cpack,
                                         bool isIgnoreChildNodesError,
                                         const std::vector<const NodeBase *> &childNodes,
                                         const ASTNodeId::ASTNodeId &astNodeId) const {
        tokenReader.push();
        std::vector<ASTNode> childASTNodes;
        for (const auto &item: childNodes) {
            tokenReader.push();
            tokenReader.push();
            DEBUG_GET_NODE_BEGIN(item)
            ASTNode astNode = item->getASTNode(tokenReader, cpack);
            DEBUG_GET_NODE_END(item)
            bool isError = astNode.isError();
            const TokensView tokens = tokenReader.collect();
            if (HEDLEY_UNLIKELY(isError && (isIgnoreChildNodesError || tokens.isEmpty()))) {
                tokenReader.restore();
                break;
            }
            childASTNodes.push_back(std::move(astNode));
            tokenReader.pop();
            if (HEDLEY_UNLIKELY(isError)) {
                break;
            }
        }
        return ASTNode::andNode(this, std::move(childASTNodes), tokenReader.collect(), nullptr, astNodeId);
    }

}// namespace CHelper::Node