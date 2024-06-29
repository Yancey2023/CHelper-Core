//
// Created by Yancey on 2023/12/1.
//

#include "NodeTargetSelector.h"
#include "../util/NodeEntry.h"
#include "../util/NodeSingleSymbol.h"
#include "NodeBoolean.h"
#include "NodeFloat.h"
#include "NodeInteger.h"
#include "NodeItem.h"
#include "NodeNormalId.h"
#include "NodeRange.h"
#include "NodeRelativeFloat.h"
#include "NodeString.h"
#include "NodeText.h"

namespace CHelper::Node {

    static std::shared_ptr<NodeBase> nodePlayerName = std::make_shared<NodeString>(
            "TARGET_SELECTOR_PLAYER_NAME", "玩家名字", false, true, false);
    static std::shared_ptr<NodeBase> nodeAt = std::make_shared<NodeSingleSymbol>(
            "TARGET_SELECTOR_AT", "@符号", '@');
    static std::shared_ptr<NodeBase> nodeTargetSelectorVariable = std::make_shared<NodeNormalId>(
            "TARGET_SELECTOR_VARIABLE", "目标选择器变量", true,
            std::make_shared<std::vector<std::shared_ptr<NormalId>>>(std::vector<std::shared_ptr<NormalId>>{
                    NormalId::make("@e", "选择所有实体(只选择活着的实体)"),
                    NormalId::make("@a", "选择所有玩家(无论死活)"),
                    NormalId::make("@r", "选择一名随机玩家(可通过type选择非玩家实体)(只选择活着的实体)"),
                    NormalId::make("@p", "选择最近的玩家(若距离相同，会在其中选择最晚进入服务器的玩家)"),
                    NormalId::make("@s", "命令的执行者(只选择唯一一个实体)(包括已死亡玩家)"),
                    NormalId::make("@initiator", "选择当前与该NPC进行交互(在NPC内置的命令界面中使用)")}),
            false,
            [](const NodeBase *node, TokenReader &tokenReader) -> ASTNode {
                tokenReader.push();
                auto childNodes = {tokenReader.readSymbolASTNode(node), tokenReader.readStringASTNode(node)};
                return ASTNode::andNode(node, childNodes, tokenReader.collect());
            });
    static std::shared_ptr<NodeBase> nodeLeft = std::make_shared<NodeSingleSymbol>(
            "TARGET_SELECTOR_ARGUMENTS_LEFT", "目标选择器参数左括号", '[');
    static std::shared_ptr<NodeBase> nodeRight = std::make_shared<NodeSingleSymbol>(
            "TARGET_SELECTOR_ARGUMENTS_RIGHT", "目标选择器参数右括号", ']');
    static std::shared_ptr<NodeBase> nodeSeparator = std::make_shared<NodeSingleSymbol>(
            "TARGET_SELECTOR_ARGUMENTS_SEPARATOR", "目标选择器参数分隔符", ',');
    static std::unique_ptr<NodeBase> nodeEqual = std::make_unique<NodeText>(
            "TARGET_SELECTOR_ARGUMENT_EQUAL", "等于",
            NormalId::make("=", "等于"),
            [](const NodeBase *node, TokenReader &tokenReader) -> ASTNode {
                return tokenReader.readSymbolASTNode(node);
            });
    static std::unique_ptr<NodeBase> nodeNotEqual = std::make_unique<NodeText>(
            "TARGET_SELECTOR_ARGUMENT_NOT_EQUAL", "不等于",
            NormalId::make("=!", "不等于"),
            [](const NodeBase *node, TokenReader &tokenReader) -> ASTNode {
                tokenReader.push();
                auto childNodes = {tokenReader.readSymbolASTNode(node), tokenReader.readSymbolASTNode(node)};
                return ASTNode::andNode(node, childNodes, tokenReader.collect());
            });
    static std::unique_ptr<NodeBase> nodeEqualOrNotEqual = std::make_unique<NodeOr>(
            "TARGET_SELECTOR_ARGUMENT_SEPARATOR", "目标选择器参数分隔符",
            std::vector<const NodeBase *>{
                    nodeEqual.get(), nodeNotEqual.get()},
            false);
    static std::unique_ptr<NodeBase> nodeRelativeFloat = std::make_unique<NodeRelativeFloat>(
            "TARGET_SELECTOR_ARGUMENT_RELATIVE_FLOAT", "目标选择器参数(小数)", false);
    static std::unique_ptr<NodeBase> nodeInteger = std::make_unique<NodeInteger>(
            "TARGET_SELECTOR_ARGUMENT_INTEGER", "目标选择器参数(整数)", std::nullopt, std::nullopt);
    static std::unique_ptr<NodeBase> nodeFloat = std::make_unique<NodeFloat>(
            "TARGET_SELECTOR_ARGUMENT_FLOAT", "目标选择器参数(小数)", std::nullopt, std::nullopt);
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
            nodeString.get(), nodeEqualOrNotEqual.get(), nodeScoreRange.get());
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
    static std::unique_ptr<NodeBase> nodeHasPermissionState = std::make_unique<NodeNormalId>(
            "PERMISSION_STATUS", "权限状态", false,
            std::make_shared<std::vector<std::shared_ptr<NormalId>>>(std::vector<std::shared_ptr<NormalId>>{
                    NormalId::make("enabled", "启用"),
                    NormalId::make("disabled", "禁用")}));
    static std::unique_ptr<NodeBase> nodeHasPermissionEntry = std::make_unique<NodeEqualEntry>(
            "TARGET_SELECTOR_ARGUMENT_HASPERMISSION_ENTRY", "目标选择器haspermission参数内容",
            std::vector<EqualData>{
                    {"camera", "玩家能否转动相机视角", false, nodeHasPermissionState.get()},
                    {"movement", "玩家能否移动", false, nodeHasPermissionState.get()}});
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
                    nodeBoolean.get(), nodeScoreRange.get(), nodeString.get(), nodeFloat.get()},
            false);
    static std::unique_ptr<NodeBase> nodeHasPropertyEntry = std::make_unique<NodeEntry>(
            "TARGET_SELECTOR_ARGUMENT_HASPROPERTY_ENTRY", "目标选择器has_property参数内容",
            nodeString.get(), nodeEqualOrNotEqual.get(), nodeAny.get());
    static std::unique_ptr<NodeBase> nodeHasProperty = std::make_unique<NodeList>(
            "TARGET_SELECTOR_ARGUMENT_HASPERMISSION", "目标选择器has_property参数",
            nodeHasPropertyValueLeft.get(), nodeHasPropertyEntry.get(),
            nodeHasPropertyValueSeparator.get(), nodeHasPropertyValueRight.get());
    static std::unique_ptr<NodeBase> nodeHasItemData = std::make_unique<NodeInteger>(
            "TARGET_SELECTOR_ARGUMENT_HASITEM_ITEM_DATA", "目标选择器hasitem参数物品数据值", 0, 32767);
    static std::unique_ptr<NodeBase> nodeHasItemQuantity = std::make_unique<NodeRange>(
            "TARGET_SELECTOR_ARGUMENT_HASITEM_ITEM_QUANTITY", "目标选择器hasitem限制范围内的所有槽位中符合条件的物品的总和数量");
    static std::unique_ptr<NodeBase> nodeHasItemSlotRange = std::make_unique<NodeRange>(
            "TARGET_SELECTOR_ARGUMENT_HASITEM_SLOT_SLOT_RANGE", "目标选择器hasitem要检测的槽位范围");

    void NodeTargetSelector::init(const CPack &cpack) {
        nodeItem = std::make_unique<NodeNamespaceId>("ITEM_ID", "物品ID", "items", true),
        nodeFamily = std::make_unique<NodeNormalId>("FAMILIES", "族", "families", true);
        nodeGameMode = std::make_unique<NodeNormalId>("GAME_MODES", "游戏模式", "gameModes", true),
        nodeSlot = std::make_unique<NodeNormalId>("SLOT", "物品栏", "slot", true),
        nodeEntities = std::make_unique<NodeNamespaceId>("ENTITIES", "实体", "entities", true),
        nodeHasItemElement = std::make_unique<NodeEqualEntry>(
                "TARGET_SELECTOR_ARGUMENT_HASITEM_ELEMENT", "目标选择器参数值(物品检测)的内容",
                std::vector<EqualData>{
                        {"item", "要检测的物品", false, nodeItem.get()},
                        {"data", "要检测的物品的数据值", false, nodeHasItemData.get()},
                        {"quantity", "限制范围内的所有槽位中符合条件的物品的总和数量", false, nodeHasItemQuantity.get()},
                        {"location", "要检测的物品栏", false, nodeSlot.get()},
                        {"slot", "用于指定要检测的槽位范围", true, nodeHasItemSlotRange.get()}}),
        nodeHasItemList1 = std::make_unique<NodeList>(
                "TARGET_SELECTOR_ARGUMENT_HASITEM", "目标选择器参数值(物品检测)",
                nodeHasItemValueLeft1.get(), nodeHasItemElement.get(),
                nodeHasItemValueSeparator.get(), nodeHasItemValueRight1.get()),
        nodeHasItemList2 = std::make_unique<NodeList>(
                "TARGET_SELECTOR_ARGUMENT_HASITEM", "目标选择器参数值(物品检测)",
                nodeHasItemValueLeft2.get(), nodeHasItemList1.get(),
                nodeHasItemValueSeparator.get(), nodeHasItemValueRight2.get()),
        nodeHasItem = std::make_unique<NodeOr>(
                "TARGET_SELECTOR_ARGUMENT_VALUE", "目标选择器参数值(物品检测)",
                std::vector<const NodeBase *>{
                        nodeHasItemList1.get(), nodeHasItemList2.get()},
                false),
        nodeArgument = std::make_unique<NodeEqualEntry>(
                "TARGET_SELECTOR_ARGUMENT", "目标选择器单个参数",
                std::vector<EqualData>{
                        {"x", "x坐标", false, nodeRelativeFloat.get()},
                        {"y", "y坐标", false, nodeRelativeFloat.get()},
                        {"z", "z坐标", false, nodeRelativeFloat.get()},
                        {"r", "半径小于等于r", false, nodeFloat.get()},
                        {"rm", "半径大于等于rm", false, nodeFloat.get()},
                        {"dx", "x坐标差异(检查实体的腿部位置)", false, nodeFloat.get()},
                        {"dy", "y坐标差异(检查实体的腿部位置)", false, nodeFloat.get()},
                        {"dz", "z坐标差异(检查实体的腿部位置)", false, nodeFloat.get()},
                        {"scores", "分数", false, nodeScore.get()},
                        {"tag", "标签", true, nodeString.get()},
                        {"name", "名字", true, nodeString.get()},
                        {"type", "实体类型", true, nodeEntities.get()},
                        {"family", "族", true, nodeFamily.get()},
                        {"rx", "垂直旋转小于等于rx", false, nodeFloat.get()},
                        {"rxm", "垂直旋转大于等于rxm", false, nodeFloat.get()},
                        {"ry", "水平旋转大于等于ry", false, nodeFloat.get()},
                        {"rym", "水平旋转大于等于rym", false, nodeFloat.get()},
                        {"hasitem", "物品栏", false, nodeHasItem.get()},
                        {"haspermission", "权限", false, nodeHasPermission.get()},
                        {"has_property", "属性", false, nodeHasProperty.get()},
                        {"l", "经验等级小于等于l", false, nodeFloat.get()},
                        {"lm", "经验等级大于等于lm", false, nodeFloat.get()},
                        {"m", "游戏模式", true, nodeGameMode.get()},
                        {"c", "目标数量(按照距离排序)", false, nodeInteger.get()},
                }),
        nodeArguments = std::make_unique<NodeList>(
                "TARGET_SELECTOR_ARGUMENTS", "目标选择器参数",
                nodeLeft.get(), nodeArgument.get(), nodeSeparator.get(), nodeRight.get());
        nodeItem->init(cpack);
        nodeFamily->init(cpack);
        nodeGameMode->init(cpack);
        nodeSlot->init(cpack);
        nodeEntities->init(cpack);
        nodeHasItemElement->init(cpack);
        nodeHasItemList1->init(cpack);
        nodeHasItemList2->init(cpack);
        nodeHasItem->init(cpack);
        nodeArgument->init(cpack);
        nodeArguments->init(cpack);
    }

    NodeType *NodeTargetSelector::getNodeType() const {
        return NodeType::TARGET_SELECTOR.get();
    }

    ASTNode NodeTargetSelector::getASTNode(TokenReader &tokenReader, const CPack *cpack) const {
        tokenReader.skipWhitespace();
        tokenReader.push();
        DEBUG_GET_NODE_BEGIN(nodeAt)
        ASTNode at = nodeAt->getASTNodeWithNextNode(tokenReader, cpack);
        DEBUG_GET_NODE_END(nodeAt)
        tokenReader.restore();
        if (HEDLEY_UNLIKELY(at.isError())) {
            //不是@符号开头，当作玩家名处理
            DEBUG_GET_NODE_BEGIN(nodePlayerName)
            ASTNode result = getByChildNode(tokenReader, cpack, nodePlayerName.get(), "target selector player name");
            DEBUG_GET_NODE_END(nodePlayerName)
            return result;
        }
        //@符号开头，进入目标选择器检测
        //目标选择器变量
        tokenReader.push();
        DEBUG_GET_NODE_BEGIN(nodeTargetSelectorVariable)
        ASTNode targetSelectorVariable = nodeTargetSelectorVariable->getASTNodeWithNextNode(tokenReader, cpack);
        DEBUG_GET_NODE_END(nodeTargetSelectorVariable)
        tokenReader.push();
        DEBUG_GET_NODE_BEGIN(nodeLeft)
        ASTNode leftBracket = nodeLeft->getASTNodeWithNextNode(tokenReader, cpack);
        DEBUG_GET_NODE_END(nodeLeft)
        tokenReader.restore();
        if (HEDLEY_LIKELY(leftBracket.isError())) {
            //没有后面的[...]
            return ASTNode::andNode(this, {targetSelectorVariable}, tokenReader.collect(),
                                    nullptr, "target selector no arguments");
        }
        ASTNode arguments = nodeArguments->getASTNodeWithNextNode(tokenReader, cpack);
        return ASTNode::andNode(this, {targetSelectorVariable, arguments}, tokenReader.collect(),
                                nullptr, "target selector with arguments");
    }

    bool NodeTargetSelector::collectSuggestions(const ASTNode *astNode,
                                                size_t index,
                                                std::vector<Suggestions> &suggestions) const {
        if (HEDLEY_UNLIKELY(astNode->tokens.isEmpty())) {
            VectorView<Token> tokens = {astNode->tokens.vector, astNode->tokens.end, astNode->tokens.end};
            ASTNode newAstNode = ASTNode::simpleNode(this, tokens);
            nodeTargetSelectorVariable->collectSuggestions(astNode, index, suggestions);
            nodePlayerName->collectSuggestions(astNode, index, suggestions);
            return true;
        }
        if (HEDLEY_UNLIKELY(!astNode->isError() && astNode->id == "target selector no arguments")) {
            VectorView<Token> tokens = {astNode->tokens.vector, astNode->tokens.end, astNode->tokens.end};
            ASTNode newAstNode = ASTNode::simpleNode(this, tokens);
            nodeLeft->collectSuggestions(&newAstNode, index, suggestions);
        }
        return false;
    }

    void NodeTargetSelector::collectStructure(const ASTNode *astNode,
                                              StructureBuilder &structure,
                                              bool isMustHave) const {
        structure.append(isMustHave, "目标选择器");
    }

    CODEC_NODE(NodeTargetSelector, isMustPlayer, isMustNPC, isOnlyOne)

}// namespace CHelper::Node