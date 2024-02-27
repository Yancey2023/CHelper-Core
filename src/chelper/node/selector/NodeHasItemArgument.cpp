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

    static std::shared_ptr<NodeBase> nodeKey = std::make_shared<NodeNormalId>(
            "TARGET_SELECTOR_ARGUMENT_HASITEM_KEY", "目标选择器hasitem参数名", std::nullopt,
            std::make_shared<std::vector<std::shared_ptr<NormalId>>>(std::vector<std::shared_ptr<NormalId>>{
                    std::make_shared<NormalId>("item", "要检测的物品"),
                    std::make_shared<NormalId>("data", "要检测的物品的数据值"),
                    std::make_shared<NormalId>("quantity", "限制范围内的所有槽位中符合条件的物品的总和数量"),
                    std::make_shared<NormalId>("location", "要检测的物品栏"),
                    std::make_shared<NormalId>("slot", "用于指定要检测的槽位范围")
            })
    );
    static std::shared_ptr<NodeBase> nodeEqual = std::make_shared<NodeText>(
            "TARGET_SELECTOR_ARGUMENT_EQUAL", "等于",
            std::make_shared<NormalId>("=", "等于"),
            [](const NodeBase *node, TokenReader &tokenReader) -> ASTNode {
                return tokenReader.readSymbolASTNode(node);
            });
    static std::shared_ptr<NodeBase> nodeNotEqual = std::make_shared<NodeText>(
            "TARGET_SELECTOR_ARGUMENT_NOT_EQUAL", "不等于",
            std::make_shared<NormalId>("=!", "不等于"),
            [](const NodeBase *node, TokenReader &tokenReader) -> ASTNode {
                tokenReader.push();
                auto childNodes = {tokenReader.readSymbolASTNode(node), tokenReader.readSymbolASTNode(node)};
                return ASTNode::andNode(node, childNodes, tokenReader.collect());
            });
    static std::shared_ptr<NodeBase> nodeSeparator = std::make_shared<NodeOr>(
            "TARGET_SELECTOR_ARGUMENT_SEPARATOR", "目标选择器参数分隔符",
            std::make_shared<std::vector<std::shared_ptr<NodeBase>>>(std::vector<std::shared_ptr<NodeBase>>{
                    nodeEqual, nodeNotEqual
            }), false);
    static std::shared_ptr<NodeBase> nodeData = std::make_shared<NodeInteger>(
            "ITEM_DATA", "物品数据值", 0, 32767);
    static std::shared_ptr<NodeBase> nodeQuantity = std::make_shared<NodeRange>(
            "ITEM_QUANTITY", "限制范围内的所有槽位中符合条件的物品的总和数量");
    static std::shared_ptr<NodeBase> nodeSlotRange = std::make_shared<NodeRange>(
            "SLOT_SLOT_RANGE", "要检测的槽位范围");
    static std::shared_ptr<NodeBase> nodeValue = std::make_shared<NodeOr>(
            "TARGET_SELECTOR_ARGUMENT_VALUE", "目标选择器参数值",
            std::make_shared<std::vector<std::shared_ptr<NodeBase>>>(std::vector<std::shared_ptr<NodeBase>>{
                    nodeData, nodeQuantity, nodeSlotRange
            }), false);

    NodeHasItemArgument::NodeHasItemArgument(const std::optional<std::string> &id,
                                             const std::optional<std::string> &description,
                                             const std::shared_ptr<NodeBase> &nodeItem,
                                             const std::shared_ptr<NodeBase> &nodeSlot)
            : NodeBase(id, description, false),
              nodeItem(nodeItem),
              nodeSlot(nodeSlot) {}

    ASTNode NodeHasItemArgument::getASTNode(TokenReader &tokenReader) const {
        tokenReader.push();
        std::vector<ASTNode> childNodes;
        // key
        ASTNode astNodeKey = getByChildNode(tokenReader, nodeKey, "key");
        childNodes.push_back(astNodeKey);
        if (astNodeKey.isError()) {
            return ASTNode::andNode(this, childNodes, tokenReader.collect());
        }
        // = or !=
        ASTNode astNodeSeparator = getByChildNode(tokenReader, nodeSeparator, "separator");
        childNodes.push_back(astNodeSeparator);
        if (astNodeSeparator.isError()) {
            return ASTNode::andNode(this, childNodes, tokenReader.collect());
        }
        //value
        std::string key = TokenUtil::toString(astNodeKey.tokens);
        if (key == "item") {
            childNodes.push_back(nodeItem->getASTNode(tokenReader));
        } else if (key == "data") {
            childNodes.push_back(nodeData->getASTNode(tokenReader));
        } else if (key == "quantity") {
            childNodes.push_back(nodeQuantity->getASTNode(tokenReader));
        } else if (key == "location") {
            childNodes.push_back(nodeSlot->getASTNode(tokenReader));
        } else if (key == "slot") {
            childNodes.push_back(nodeSlotRange->getASTNode(tokenReader));
        } else {
            childNodes.push_back(nodeValue->getASTNode(tokenReader));
        }
        return ASTNode::andNode(this, childNodes, tokenReader.collect());
    }

} // CHelper::Node