//
// Created by Yancey on 2024/2/20.
//

#include "NodeTargetSelectorArgument.h"
#include "../param/NodeString.h"
#include "../param/NodeText.h"
#include "../../util/TokenUtil.h"
#include "../param/NodeRelativeFloat.h"
#include "../param/NodeInteger.h"
#include "../util/NodeSingleSymbol.h"
#include "../util/NodeEntry.h"
#include "../param/NodeRange.h"
#include "NodeHasPermissionArgument.h"
#include "../param/NodeBoolean.h"

namespace CHelper::Node {

    static std::unique_ptr<NodeBase> nodeKey = std::make_unique<NodeNormalId>(
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
    static std::unique_ptr<NodeBase> nodeRelativeFloat = std::make_unique<NodeRelativeFloat>(
            "TARGET_SELECTOR_ARGUMENT_RELATIVE_FLOAT", "目标选择器参数(小数)", true);
    static std::unique_ptr<NodeBase> nodeInteger = std::make_unique<NodeInteger>(
            "TARGET_SELECTOR_ARGUMENT_FLOAT", "目标选择器参数(小数)", std::nullopt, std::nullopt);
    static std::unique_ptr<NodeBase> nodeFloat = std::make_unique<NodeInteger>(
            "TARGET_SELECTOR_ARGUMENT_INTEGER", "目标选择器参数(整数)", std::nullopt, std::nullopt);
    static std::unique_ptr<NodeBase> nodeString = std::make_unique<NodeString>(
            "TARGET_SELECTOR_ARGUMENT_STRING", "目标选择器参数(字符串)", true, true, false);
    static std::unique_ptr<NodeBase> nodeScoreValueLeft = std::make_unique<NodeSingleSymbol>(
            "TARGET_SELECTOR_ARGUMENT_SCORES_LEFT", "目标选择器scores参数左括号", '{');
    static std::unique_ptr<NodeBase> nodeScoreValueRight = std::make_unique<NodeSingleSymbol>(
            "TARGET_SELECTOR_ARGUMENT_SCORES_RIGHT", "目标选择器scores参数右括号", '}');
    static std::unique_ptr<NodeBase> nodeScoreValueSeparator = std::make_unique<NodeSingleSymbol>(
            "TARGET_SELECTOR_ARGUMENT_SCORES_SEPARATOR", "目标选择器scores参数分隔符", ',');
    static std::unique_ptr<NodeBase> nodeScoreRange = std::make_unique<NodeRange>(
            "TARGET_SELECTOR_ARGUMENT_SCORES_RANGE", "目标选择器scores参数值的分数范围)");
    static std::unique_ptr<NodeBase> nodeScoreValueElement = std::make_unique<NodeEntry>(
            "TARGET_SELECTOR_ARGUMENT_SCORES_ELEMENT", "目标选择器scores参数内容",
            nodeString.get(), nodeSeparator.get(), nodeScoreRange.get());
    static std::unique_ptr<NodeBase> nodeScore = std::make_unique<NodeList>(
            "TARGET_SELECTOR_ARGUMENT_SCORES", "目标选择器scores参数",
            nodeScoreValueLeft.get(), nodeScoreValueElement.get(),
            nodeScoreValueSeparator.get(), nodeScoreValueRight.get());
    static std::unique_ptr<NodeBase> nodeHasItemValueLeft1 = std::make_unique<NodeSingleSymbol>(
            "TARGET_SELECTOR_ARGUMENT_HASITEM_LEFT1", "目标选择器hasitem参数左括号", '{');
    static std::unique_ptr<NodeBase> nodeHasItemValueRight1 = std::make_unique<NodeSingleSymbol>(
            "TARGET_SELECTOR_ARGUMENT_HASITEM_RIGHT1", "目标选择器hasitem参数右括号", '}');
    static std::unique_ptr<NodeBase> nodeHasItemValueSeparator = std::make_unique<NodeSingleSymbol>(
            "TARGET_SELECTOR_ARGUMENT_HASITEM_SEPARATOR", "目标选择器hasitem参数分隔符", ',');
    static std::unique_ptr<NodeBase> nodeHasItemValueLeft2 = std::make_unique<NodeSingleSymbol>(
            "TARGET_SELECTOR_ARGUMENT_HASITEM_LEFT2", "目标选择器hasitem参数左括号", '[');
    static std::unique_ptr<NodeBase> nodeHasItemValueRight2 = std::make_unique<NodeSingleSymbol>(
            "TARGET_SELECTOR_ARGUMENT_HASITEM_RIGHT2", "目标选择器haspermission参数右括号", ']');
    static std::unique_ptr<NodeBase> nodeHasPermissionValueLeft = std::make_unique<NodeSingleSymbol>(
            "TARGET_SELECTOR_ARGUMENT_HASPERMISSION_LEFT", "目标选择器haspermission参数左括号", '{');
    static std::unique_ptr<NodeBase> nodeHasPermissionValueRight = std::make_unique<NodeSingleSymbol>(
            "TARGET_SELECTOR_ARGUMENT_HASPERMISSION_RIGHT", "目标选择器haspermission参数右括号", '}');
    static std::unique_ptr<NodeBase> nodeHasPermissionValueSeparator = std::make_unique<NodeSingleSymbol>(
            "TARGET_SELECTOR_ARGUMENT_HASPERMISSION_SEPARATOR", "目标选择器haspermission参数分隔符", ',');
    static std::unique_ptr<NodeBase> nodeHasPermissionEntry = std::make_unique<NodeHasPermissionArgument>(
            "TARGET_SELECTOR_ARGUMENT_HASPERMISSION_ENTRY", "目标选择器haspermission参数内容");
    static std::unique_ptr<NodeBase> nodeHasPermission = std::make_unique<NodeList>(
            "TARGET_SELECTOR_ARGUMENT_HASPERMISSION", "目标选择器haspermission参数",
            nodeHasPermissionValueLeft.get(), nodeHasPermissionEntry.get(),
            nodeHasPermissionValueSeparator.get(), nodeHasPermissionValueRight.get());
    static std::unique_ptr<NodeBase> nodeHasPropertyValueLeft = std::make_unique<NodeSingleSymbol>(
            "TARGET_SELECTOR_ARGUMENT_HASPROPERTY_LEFT", "目标选择器has_property参数左括号", '{');
    static std::unique_ptr<NodeBase> nodeHasPropertyValueRight = std::make_unique<NodeSingleSymbol>(
            "TARGET_SELECTOR_ARGUMENT_HASPROPERTY_RIGHT", "目标选择器has_property参数右括号", '}');
    static std::unique_ptr<NodeBase> nodeHasPropertyValueSeparator = std::make_unique<NodeSingleSymbol>(
            "TARGET_SELECTOR_ARGUMENT_HASPROPERTY_SEPARATOR", "目标选择器has_property参数分隔符", ',');
    static std::unique_ptr<NodeBase> nodeBoolean = std::make_unique<NodeBoolean>(
            "BOOLEAN", "布尔值", std::nullopt, std::nullopt);
    static std::unique_ptr<NodeBase> nodeAny = std::make_unique<NodeOr>(
            "ANY", "任何值",
            std::vector<const NodeBase *>{
                    nodeBoolean.get(), nodeScoreRange.get(), nodeString.get(), nodeFloat.get()
            }, false);
    static std::unique_ptr<NodeBase> nodeHasPropertyEntry = std::make_unique<NodeEntry>(
            "TARGET_SELECTOR_ARGUMENT_HASPROPERTY_ENTRY", "目标选择器has_property参数内容",
            nodeString.get(), nodeSeparator.get(), nodeAny.get());
    static std::unique_ptr<NodeBase> nodeHasProperty = std::make_unique<NodeList>(
            "TARGET_SELECTOR_ARGUMENT_HASPERMISSION", "目标选择器has_property参数",
            nodeHasPropertyValueLeft.get(), nodeHasPropertyEntry.get(),
            nodeHasPropertyValueSeparator.get(), nodeHasPropertyValueRight.get());
    static std::unique_ptr<NodeBase> nodeValue = std::make_unique<NodeOr>(
            "TARGET_SELECTOR_ARGUMENT_VALUE", "目标选择器参数值",
            std::vector<const NodeBase *>{
                    nodeRelativeFloat.get(), nodeInteger.get(), nodeInteger.get(), nodeString.get(), nodeScore.get()
            }, false);

    [[maybe_unused]] NodeTargetSelectorArgument::NodeTargetSelectorArgument(const std::optional<std::string> &id,
                                                                            const std::optional<std::string> &description,
                                                                            const NodeBase *nodeItem,
                                                                            const NodeBase *nodeFamily,
                                                                            const NodeBase *nodeGameMode,
                                                                            const NodeBase *nodeSlot,
                                                                            const NodeBase *nodeEntities)
            : NodeBase(id, description, false),
              nodeHasItemElement(
                      "TARGET_SELECTOR_ARGUMENT_HASITEM_ELEMENT", "目标选择器参数值(物品检测)的内容",
                      nodeItem, nodeSlot),
              nodeHasItemList1(
                      "TARGET_SELECTOR_ARGUMENT_HASITEM", "目标选择器参数值(物品检测)",
                      nodeHasItemValueLeft1.get(), &nodeHasItemElement,
                      nodeHasItemValueSeparator.get(), nodeHasItemValueRight1.get()),
              nodeHasItemList2(
                      "TARGET_SELECTOR_ARGUMENT_HASITEM", "目标选择器参数值(物品检测)",
                      nodeHasItemValueLeft2.get(), &nodeHasItemList1,
                      nodeHasItemValueSeparator.get(), nodeHasItemValueRight2.get()),
              nodeHasItem(
                      "TARGET_SELECTOR_ARGUMENT_VALUE", "目标选择器参数值(物品检测)",
                      std::vector<const NodeBase *>{
                              &nodeHasItemList1, &nodeHasItemList2
                      }, false),
              nodeFamily(nodeFamily),
              nodeGameMode(nodeGameMode),
              nodeEntities(nodeEntities) {}

    ASTNode NodeTargetSelectorArgument::getASTNode(TokenReader &tokenReader, const CPack *cpack) const {
        tokenReader.push();
        std::vector<ASTNode> childNodes;
        // key
        ASTNode astNodeKey = getByChildNode(tokenReader, cpack, nodeKey.get(), "key");
        childNodes.push_back(astNodeKey);
        if (HEDLEY_UNLIKELY(astNodeKey.isError())) {
            return ASTNode::andNode(this, std::move(childNodes), tokenReader.collect());
        }
        // = or !=
        ASTNode astNodeSeparator = getByChildNode(tokenReader, cpack, nodeSeparator.get(), "separator");
        childNodes.push_back(astNodeSeparator);
        if (HEDLEY_UNLIKELY(astNodeSeparator.isError())) {
            return ASTNode::andNode(this, std::move(childNodes), tokenReader.collect());
        }
        //value
        std::string key = TokenUtil::toString(astNodeKey.tokens);
        if (HEDLEY_LIKELY(key == "x" || key == "y" || key == "z")) {
            childNodes.push_back(nodeRelativeFloat->getASTNodeWithNextNode(tokenReader, cpack));
        } else if (HEDLEY_LIKELY(key == "r" || key == "rm" || key == "dx" || key == "dy" || key == "dz" ||
                                 key == "rx" || key == "rxm" || key == "ry" || key == "rym" ||
                                 key == "l" || key == "lm")) {
            childNodes.push_back(nodeFloat->getASTNodeWithNextNode(tokenReader, cpack));
        } else if (HEDLEY_LIKELY(key == "scores")) {
            childNodes.push_back(nodeScore->getASTNodeWithNextNode(tokenReader, cpack));
        } else if (HEDLEY_LIKELY(key == "tag" || key == "name")) {
            childNodes.push_back(nodeString->getASTNodeWithNextNode(tokenReader, cpack));
        } else if (HEDLEY_LIKELY(key == "type")) {
            childNodes.push_back(nodeEntities->getASTNodeWithNextNode(tokenReader, cpack));
        } else if (HEDLEY_LIKELY(key == "family")) {
            childNodes.push_back(nodeFamily->getASTNodeWithNextNode(tokenReader, cpack));
        } else if (HEDLEY_LIKELY(key == "hasitem")) {
            childNodes.push_back(nodeHasItem.getASTNodeWithNextNode(tokenReader, cpack));
        } else if (HEDLEY_LIKELY(key == "has_property")) {
            childNodes.push_back(nodeHasProperty->getASTNodeWithNextNode(tokenReader, cpack));
        } else if (HEDLEY_LIKELY(key == "haspermission")) {
            childNodes.push_back(nodeHasPermission->getASTNodeWithNextNode(tokenReader, cpack));
        } else if (HEDLEY_LIKELY(key == "m")) {
            childNodes.push_back(nodeGameMode->getASTNodeWithNextNode(tokenReader, cpack));
        } else if (HEDLEY_LIKELY(key == "c")) {
            childNodes.push_back(nodeInteger->getASTNodeWithNextNode(tokenReader, cpack));
        } else {
            childNodes.push_back(nodeValue->getASTNodeWithNextNode(tokenReader, cpack));
        }
        return ASTNode::andNode(this, std::move(childNodes), tokenReader.collect());
    }

} // CHelper::Node