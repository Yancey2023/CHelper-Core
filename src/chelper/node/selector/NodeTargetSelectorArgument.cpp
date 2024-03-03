//
// Created by Yancey on 2024/2/20.
//

#include "NodeTargetSelectorArgument.h"
#include "../param/NodeString.h"
#include "../param/NodeText.h"
#include "../../util/TokenUtil.h"
#include "../param/NodeNormalId.h"
#include "../param/NodeFloat.h"
#include "../param/NodeRelativeFloat.h"
#include "../util/NodeOr.h"
#include "../param/NodeInteger.h"
#include "../util/NodeList.h"
#include "../util/NodeSingleSymbol.h"
#include "../util/NodeEntry.h"
#include "../param/NodeRange.h"
#include "NodeHasItemArgument.h"
#include "NodeHasPermissionArgument.h"
#include "../param/NodeBoolean.h"

namespace CHelper::Node {

    static std::shared_ptr<NodeBase> nodeKey = std::make_shared<NodeNormalId>(
            "TARGET_SELECTOR_ARGUMENT_KEY", "目标选择器参数名", std::nullopt, true,
            std::make_shared<std::vector<std::shared_ptr<NormalId>>>(std::vector<std::shared_ptr<NormalId>>{
                    std::make_shared<NormalId>("x", "x坐标"),
                    std::make_shared<NormalId>("y", "y坐标"),
                    std::make_shared<NormalId>("z", "z坐标"),
                    std::make_shared<NormalId>("r", "半径小于等于r"),
                    std::make_shared<NormalId>("rm", "半径大于等于rm"),
                    std::make_shared<NormalId>("dx", "x坐标差异(检查实体的腿部位置)"),
                    std::make_shared<NormalId>("dy", "y坐标差异(检查实体的腿部位置)"),
                    std::make_shared<NormalId>("dz", "z坐标差异(检查实体的腿部位置)"),
                    std::make_shared<NormalId>("scores", "分数"),
                    std::make_shared<NormalId>("tag", "标签"),
                    std::make_shared<NormalId>("name", "名字"),
                    std::make_shared<NormalId>("type", "实体类型"),
                    std::make_shared<NormalId>("family", "族"),
                    std::make_shared<NormalId>("rx", "垂直旋转小于等于rx"),
                    std::make_shared<NormalId>("rxm", "垂直旋转大于等于rxm"),
                    std::make_shared<NormalId>("ry", "水平旋转大于等于ry"),
                    std::make_shared<NormalId>("rym", "水平旋转大于等于rym"),
                    std::make_shared<NormalId>("hasitem", "物品栏"),
                    std::make_shared<NormalId>("haspermission", "权限"),
                    std::make_shared<NormalId>("has_property", "属性"),
                    std::make_shared<NormalId>("l", "经验等级小于等于l"),
                    std::make_shared<NormalId>("lm", "经验等级大于等于lm"),
                    std::make_shared<NormalId>("m", "游戏模式"),
                    std::make_shared<NormalId>("c", "目标数量(按照距离排序)"),
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
    static std::shared_ptr<NodeBase> nodeRelativeFloat = std::make_shared<NodeRelativeFloat>(
            "TARGET_SELECTOR_ARGUMENT_RELATIVE_FLOAT", "目标选择器参数(小数)", true);
    static std::shared_ptr<NodeBase> nodeInteger = std::make_shared<NodeInteger>(
            "TARGET_SELECTOR_ARGUMENT_FLOAT", "目标选择器参数(小数)", std::nullopt, std::nullopt);
    static std::shared_ptr<NodeBase> nodeFloat = std::make_shared<NodeInteger>(
            "TARGET_SELECTOR_ARGUMENT_INTEGER", "目标选择器参数(整数)", std::nullopt, std::nullopt);
    static std::shared_ptr<NodeBase> nodeString = std::make_shared<NodeString>(
            "TARGET_SELECTOR_ARGUMENT_STRING", "目标选择器参数(字符串)", true, true, false);
    static std::shared_ptr<NodeBase> nodeScoreValueLeft = std::make_shared<NodeSingleSymbol>(
            "TARGET_SELECTOR_ARGUMENT_SCORES_LEFT", "目标选择器scores参数左括号", '{');
    static std::shared_ptr<NodeBase> nodeScoreValueRight = std::make_shared<NodeSingleSymbol>(
            "TARGET_SELECTOR_ARGUMENT_SCORES_RIGHT", "目标选择器scores参数右括号", '}');
    static std::shared_ptr<NodeBase> nodeScoreValueSeparator = std::make_shared<NodeSingleSymbol>(
            "TARGET_SELECTOR_ARGUMENT_SCORES_SEPARATOR", "目标选择器scores参数分隔符", ',');
    static std::shared_ptr<NodeBase> nodeScoreRange = std::make_shared<NodeRange>(
            "TARGET_SELECTOR_ARGUMENT_SCORES_RANGE", "目标选择器scores参数值的分数范围)");
    static std::shared_ptr<NodeBase> nodeScoreValueElement = std::make_shared<NodeEntry>(
            "TARGET_SELECTOR_ARGUMENT_SCORES_ELEMENT", "目标选择器scores参数内容",
            nodeString, nodeSeparator, nodeScoreRange);
    static std::shared_ptr<NodeBase> nodeScore = std::make_shared<NodeList>(
            "TARGET_SELECTOR_ARGUMENT_SCORES", "目标选择器scores参数",
            nodeScoreValueLeft, nodeScoreValueElement, nodeScoreValueSeparator, nodeScoreValueRight);
    static std::shared_ptr<NodeBase> nodeHasItemValueLeft1 = std::make_shared<NodeSingleSymbol>(
            "TARGET_SELECTOR_ARGUMENT_HASITEM_LEFT1", "目标选择器hasitem参数左括号", '{');
    static std::shared_ptr<NodeBase> nodeHasItemValueRight1 = std::make_shared<NodeSingleSymbol>(
            "TARGET_SELECTOR_ARGUMENT_HASITEM_RIGHT1", "目标选择器hasitem参数右括号", '}');
    static std::shared_ptr<NodeBase> nodeHasItemValueSeparator = std::make_shared<NodeSingleSymbol>(
            "TARGET_SELECTOR_ARGUMENT_HASITEM_SEPARATOR", "目标选择器hasitem参数分隔符", ',');
    static std::shared_ptr<NodeBase> nodeHasItemValueLeft2 = std::make_shared<NodeSingleSymbol>(
            "TARGET_SELECTOR_ARGUMENT_HASITEM_LEFT2", "目标选择器hasitem参数左括号", '[');
    static std::shared_ptr<NodeBase> nodeHasItemValueRight2 = std::make_shared<NodeSingleSymbol>(
            "TARGET_SELECTOR_ARGUMENT_HASITEM_RIGHT2", "目标选择器haspermission参数右括号", ']');
    static std::shared_ptr<NodeBase> nodeHasPermissionValueLeft = std::make_shared<NodeSingleSymbol>(
            "TARGET_SELECTOR_ARGUMENT_HASPERMISSION_LEFT", "目标选择器haspermission参数左括号", '{');
    static std::shared_ptr<NodeBase> nodeHasPermissionValueRight = std::make_shared<NodeSingleSymbol>(
            "TARGET_SELECTOR_ARGUMENT_HASPERMISSION_RIGHT", "目标选择器haspermission参数右括号", '}');
    static std::shared_ptr<NodeBase> nodeHasPermissionValueSeparator = std::make_shared<NodeSingleSymbol>(
            "TARGET_SELECTOR_ARGUMENT_HASPERMISSION_SEPARATOR", "目标选择器haspermission参数分隔符", ',');
    static std::shared_ptr<NodeBase> nodeHasPermissionEntry = std::make_shared<NodeHasPermissionArgument>(
            "TARGET_SELECTOR_ARGUMENT_HASPERMISSION_ENTRY", "目标选择器haspermission参数内容");
    static std::shared_ptr<NodeBase> nodeHasPermission = std::make_shared<NodeList>(
            "TARGET_SELECTOR_ARGUMENT_HASPERMISSION", "目标选择器haspermission参数",
            nodeHasPermissionValueLeft, nodeHasPermissionEntry,
            nodeHasPermissionValueSeparator, nodeHasPermissionValueRight);
    static std::shared_ptr<NodeBase> nodeHasPropertyValueLeft = std::make_shared<NodeSingleSymbol>(
            "TARGET_SELECTOR_ARGUMENT_HASPROPERTY_LEFT", "目标选择器haspermission参数左括号", '{');
    static std::shared_ptr<NodeBase> nodeHasPropertyValueRight = std::make_shared<NodeSingleSymbol>(
            "TARGET_SELECTOR_ARGUMENT_HASPROPERTY_RIGHT", "目标选择器haspermission参数右括号", '}');
    static std::shared_ptr<NodeBase> nodeHasPropertyValueSeparator = std::make_shared<NodeSingleSymbol>(
            "TARGET_SELECTOR_ARGUMENT_HASPROPERTY_SEPARATOR", "目标选择器haspermission参数分隔符", ',');
    static std::shared_ptr<NodeBase> nodeBoolean = std::make_shared<NodeBoolean>(
            "BOOLEAN", "布尔值", std::nullopt, std::nullopt);
    static std::shared_ptr<NodeBase> nodeAny = std::make_shared<NodeOr>(
            "ANY", "任何值",
            std::make_shared<std::vector<std::shared_ptr<NodeBase>>>(std::vector<std::shared_ptr<NodeBase>>{
                    nodeBoolean, nodeScoreRange, nodeString, nodeFloat
            }), false);
    static std::shared_ptr<NodeBase> nodeHasPropertyEntry = std::make_shared<NodeEntry>(
            "TARGET_SELECTOR_ARGUMENT_HASPROPERTY_ENTRY", "目标选择器haspermission参数内容",
            nodeString, nodeSeparator, nodeAny);
    static std::shared_ptr<NodeBase> nodeHasProperty = std::make_shared<NodeList>(
            "TARGET_SELECTOR_ARGUMENT_HASPERMISSION", "目标选择器hasproperty参数",
            nodeHasPropertyValueLeft, nodeHasPropertyEntry,
            nodeHasPropertyValueSeparator, nodeHasPropertyValueRight);
    static std::shared_ptr<NodeBase> nodeValue = std::make_shared<NodeOr>(
            "TARGET_SELECTOR_ARGUMENT_VALUE", "目标选择器参数值",
            std::make_shared<std::vector<std::shared_ptr<NodeBase>>>(std::vector<std::shared_ptr<NodeBase>>{
                    nodeRelativeFloat, nodeInteger, nodeInteger, nodeString, nodeScore
            }), false);

    NodeTargetSelectorArgument::NodeTargetSelectorArgument(const std::optional<std::string> &id,
                                                           const std::optional<std::string> &description,
                                                           const std::shared_ptr<NodeBase> &nodeItem,
                                                           const std::shared_ptr<NodeBase> &nodeFamily,
                                                           const std::shared_ptr<NodeBase> &nodeGameMode,
                                                           const std::shared_ptr<NodeBase> &nodeSlot,
                                                           const std::shared_ptr<NodeBase> &nodeEntities)
            : NodeBase(id, description, false),
              nodeHasItemElement(std::make_shared<NodeHasItemArgument>(
                      "TARGET_SELECTOR_ARGUMENT_HASITEM_ELEMENT", "目标选择器参数值(物品检测)的内容",
                      nodeItem, nodeSlot)),
              nodeHasItemList1(std::make_shared<NodeList>(
                      "TARGET_SELECTOR_ARGUMENT_HASITEM", "目标选择器参数值(物品检测)",
                      nodeHasItemValueLeft1, nodeHasItemElement, nodeHasItemValueSeparator, nodeHasItemValueRight1)),
              nodeHasItemList2(std::make_shared<NodeList>(
                      "TARGET_SELECTOR_ARGUMENT_HASITEM", "目标选择器参数值(物品检测)",
                      nodeHasItemValueLeft2, nodeHasItemList1, nodeHasItemValueSeparator, nodeHasItemValueRight2)),
              nodeHasItem(std::make_shared<NodeOr>(
                      "TARGET_SELECTOR_ARGUMENT_VALUE", "目标选择器参数值(物品检测)",
                      std::make_shared<std::vector<std::shared_ptr<NodeBase>>>(std::vector<std::shared_ptr<NodeBase>>{
                              nodeHasItemList1, nodeHasItemList2
                      }), false)),
              nodeFamily(nodeFamily),
              nodeGameMode(nodeGameMode),
              nodeEntities(nodeEntities) {}

    ASTNode NodeTargetSelectorArgument::getASTNode(TokenReader &tokenReader, const CPack &cpack) const {
        tokenReader.push();
        std::vector<ASTNode> childNodes;
        // key
        ASTNode astNodeKey = getByChildNode(tokenReader, cpack, nodeKey, "key");
        childNodes.push_back(astNodeKey);
        if (astNodeKey.isError()) {
            return ASTNode::andNode(this, childNodes, tokenReader.collect());
        }
        // = or !=
        ASTNode astNodeSeparator = getByChildNode(tokenReader, cpack, nodeSeparator, "separator");
        childNodes.push_back(astNodeSeparator);
        if (astNodeSeparator.isError()) {
            return ASTNode::andNode(this, childNodes, tokenReader.collect());
        }
        //value
        std::string key = TokenUtil::toString(astNodeKey.tokens);
        if (key == "x" || key == "y" || key == "z") {
            childNodes.push_back(nodeRelativeFloat->getASTNodeWithNextNode(tokenReader, cpack));
        } else if (key == "r" || key == "rm" || key == "dx" || key == "dy" || key == "dz" ||
                   key == "rx" || key == "rxm" || key == "ry" || key == "rym" ||
                   key == "l" || key == "lm") {
            childNodes.push_back(nodeFloat->getASTNodeWithNextNode(tokenReader, cpack));
        } else if (key == "scores") {
            childNodes.push_back(nodeScore->getASTNodeWithNextNode(tokenReader, cpack));
        } else if (key == "tag" || key == "name") {
            childNodes.push_back(nodeString->getASTNodeWithNextNode(tokenReader, cpack));
        } else if (key == "type") {
            childNodes.push_back(nodeEntities->getASTNodeWithNextNode(tokenReader, cpack));
        } else if (key == "family") {
            childNodes.push_back(nodeFamily->getASTNodeWithNextNode(tokenReader, cpack));
        } else if (key == "hasitem") {
            childNodes.push_back(nodeHasItem->getASTNodeWithNextNode(tokenReader, cpack));
        } else if (key == "has_property") {
            childNodes.push_back(nodeHasProperty->getASTNodeWithNextNode(tokenReader, cpack));
        } else if (key == "haspermission") {
            childNodes.push_back(nodeHasPermission->getASTNodeWithNextNode(tokenReader, cpack));
        } else if (key == "m") {
            childNodes.push_back(nodeGameMode->getASTNodeWithNextNode(tokenReader, cpack));
        } else if (key == "c") {
            childNodes.push_back(nodeInteger->getASTNodeWithNextNode(tokenReader, cpack));
        } else {
            childNodes.push_back(nodeValue->getASTNodeWithNextNode(tokenReader, cpack));
        }
        return ASTNode::andNode(this, childNodes, tokenReader.collect());
    }

} // CHelper::Node