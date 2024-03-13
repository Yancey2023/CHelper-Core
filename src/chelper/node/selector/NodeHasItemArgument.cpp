//
// Created by Yancey on 2024/2/27.
//

#include "NodeHasItemArgument.h"
#include "../param/NodeNamespaceId.h"
#include "../param/NodeInteger.h"
#include "../param/NodeRange.h"
#include "../param/NodeString.h"
#include "../param/NodeNormalId.h"
#include "../param/NodeText.h"
#include "../../util/TokenUtil.h"

namespace CHelper::Node {

    static std::unique_ptr<NodeBase> nodeKey = std::make_unique<NodeNormalId>(
            "TARGET_SELECTOR_ARGUMENT_HASITEM_KEY", "目标选择器hasitem参数名", std::nullopt, true,
            std::make_shared<std::vector<std::shared_ptr<NormalId>>>(std::vector<std::shared_ptr<NormalId>>{
                    std::make_shared<NormalId>("item", "要检测的物品"),
                    std::make_shared<NormalId>("data", "要检测的物品的数据值"),
                    std::make_shared<NormalId>("quantity", "限制范围内的所有槽位中符合条件的物品的总和数量"),
                    std::make_shared<NormalId>("location", "要检测的物品栏"),
                    std::make_shared<NormalId>("slot", "用于指定要检测的槽位范围")
            })
    );
    static std::unique_ptr<NodeBase> nodeEqual = std::make_unique<NodeText>(
            "TARGET_SELECTOR_ARGUMENT_EQUAL", "等于",
            std::make_shared<NormalId>("=", "等于"),
            [](const NodeBase *node, TokenReader &tokenReader) -> ASTNode {
                return tokenReader.readSymbolASTNode(node);
            });
    static std::unique_ptr<NodeBase> nodeNotEqual = std::make_unique<NodeText>(
            "TARGET_SELECTOR_ARGUMENT_NOT_EQUAL", "不等于",
            std::make_shared<NormalId>("=!", "不等于"),
            [](const NodeBase *node, TokenReader &tokenReader) -> ASTNode {
                tokenReader.push();
                auto childNodes = {tokenReader.readSymbolASTNode(node), tokenReader.readSymbolASTNode(node)};
                return ASTNode::andNode(node, childNodes, tokenReader.collect());
            });
    static std::unique_ptr<NodeBase> nodeSeparator = std::make_unique<NodeOr>(
            "TARGET_SELECTOR_ARGUMENT_SEPARATOR", "目标选择器参数分隔符",
            std::vector<const NodeBase *>{
                    nodeEqual.get(), nodeNotEqual.get()
            }, false);
    static std::unique_ptr<NodeBase> nodeData = std::make_unique<NodeInteger>(
            "ITEM_DATA", "物品数据值", 0, 32767);
    static std::unique_ptr<NodeBase> nodeQuantity = std::make_unique<NodeRange>(
            "ITEM_QUANTITY", "限制范围内的所有槽位中符合条件的物品的总和数量");
    static std::unique_ptr<NodeBase> nodeSlotRange = std::make_unique<NodeRange>(
            "SLOT_SLOT_RANGE", "要检测的槽位范围");
    static std::unique_ptr<NodeBase> nodeValue = std::make_unique<NodeOr>(
            "TARGET_SELECTOR_ARGUMENT_VALUE", "目标选择器参数值",
            std::vector<const NodeBase *>{
                    nodeData.get(), nodeQuantity.get(), nodeSlotRange.get()
            }, false);

    NodeHasItemArgument::NodeHasItemArgument(const std::optional<std::string> &id,
                                             const std::optional<std::string> &description,
                                             const NodeBase *nodeItem,
                                             const NodeBase *nodeSlot)
            : NodeBase(id, description, false),
              nodeItem(nodeItem),
              nodeSlot(nodeSlot) {}

    ASTNode NodeHasItemArgument::getASTNode(TokenReader &tokenReader, const CPack *cpack) const {
        tokenReader.push();
        std::vector<ASTNode> childNodes;
        // key
        ASTNode astNodeKey = getByChildNode(tokenReader, cpack, nodeKey.get(), "key");
        childNodes.push_back(astNodeKey);
        if (astNodeKey.isError()) {
            return ASTNode::andNode(this, std::move(childNodes), tokenReader.collect());
        }
        // = or !=
        ASTNode astNodeSeparator = getByChildNode(tokenReader, cpack, nodeSeparator.get(), "separator");
        childNodes.push_back(astNodeSeparator);
        if (astNodeSeparator.isError()) {
            return ASTNode::andNode(this, std::move(childNodes), tokenReader.collect());
        }
        //value
        std::string key = TokenUtil::toString(astNodeKey.tokens);
        if (key == "item") {
            childNodes.push_back(nodeItem->getASTNodeWithNextNode(tokenReader, cpack));
        } else if (key == "data") {
            childNodes.push_back(nodeData->getASTNodeWithNextNode(tokenReader, cpack));
        } else if (key == "quantity") {
            childNodes.push_back(nodeQuantity->getASTNodeWithNextNode(tokenReader, cpack));
        } else if (key == "location") {
            childNodes.push_back(nodeSlot->getASTNodeWithNextNode(tokenReader, cpack));
        } else if (key == "slot") {
            childNodes.push_back(nodeSlotRange->getASTNodeWithNextNode(tokenReader, cpack));
        } else {
            childNodes.push_back(nodeValue->getASTNodeWithNextNode(tokenReader, cpack));
        }
        return ASTNode::andNode(this, std::move(childNodes), tokenReader.collect());
    }

} // CHelper::Node