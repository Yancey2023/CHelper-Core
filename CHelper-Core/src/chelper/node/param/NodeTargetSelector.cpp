//
// Created by Yancey on 2023/12/1.
//

#include <chelper/node/NodeType.h>
#include <chelper/node/param/NodeNormalId.h>
#include <chelper/node/param/NodeRange.h>
#include <chelper/node/param/NodeRelativeFloat.h>
#include <chelper/node/param/NodeString.h>
#include <chelper/node/param/NodeTargetSelector.h>
#include <chelper/node/param/NodeText.h>
#include <chelper/node/template/NodeTemplateBoolean.h>
#include <chelper/node/template/NodeTemplateNumber.h>
#include <chelper/node/util/NodeEntry.h>
#include <chelper/node/util/NodeSingleSymbol.h>

namespace CHelper::Node {

    static std::shared_ptr<NodeBase> nodePlayerName = std::make_shared<NodeString>(
            u"TARGET_SELECTOR_PLAYER_NAME", u"玩家名字", false, true, false);
    static std::shared_ptr<NodeBase> nodeWildcard = std::make_shared<NodeSingleSymbol>(
            u"TARGET_SELECTOR_AT", u"所有正被记分板跟踪的实体", u'*');
    static std::shared_ptr<NodeBase> nodeAt = std::make_shared<NodeSingleSymbol>(
            u"TARGET_SELECTOR_AT", u"@符号", u'@');
    static std::shared_ptr<NodeBase> nodeTargetSelectorVariable = std::make_shared<NodeNormalId>(
            u"TARGET_SELECTOR_VARIABLE", u"目标选择器变量", true,
            std::make_shared<std::vector<std::shared_ptr<NormalId>>>(std::vector<std::shared_ptr<NormalId>>{
                    NormalId::make(u"@e", u"选择所有实体(只选择活着的实体)"),
                    NormalId::make(u"@a", u"选择所有玩家(无论死活)"),
                    NormalId::make(u"@r", u"选择一名随机玩家(可通过type选择非玩家实体)(只选择活着的实体)"),
                    NormalId::make(u"@p", u"选择最近的玩家(若距离相同，会在其中选择最晚进入服务器的玩家)"),
                    NormalId::make(u"@s", u"命令的执行者(只选择唯一一个实体)(包括已死亡玩家)"),
                    NormalId::make(u"@initiator", u"选择当前与该NPC进行交互(在NPC内置的命令界面中使用)")}),
            false,
            [](const NodeBase *node, TokenReader &tokenReader) -> ASTNode {
                tokenReader.push();
                auto childNodes = {tokenReader.readSymbolASTNode(node), tokenReader.readStringASTNode(node)};
                return ASTNode::andNode(node, childNodes, tokenReader.collect());
            });
    static std::shared_ptr<NodeBase> nodeLeft = std::make_shared<NodeSingleSymbol>(
            u"TARGET_SELECTOR_ARGUMENTS_LEFT", u"目标选择器参数左括号", u'[');
    static std::shared_ptr<NodeBase> nodeRight = std::make_shared<NodeSingleSymbol>(
            u"TARGET_SELECTOR_ARGUMENTS_RIGHT", u"目标选择器参数右括号", u']');
    static std::shared_ptr<NodeBase> nodeSeparator = std::make_shared<NodeSingleSymbol>(
            u"TARGET_SELECTOR_ARGUMENTS_SEPARATOR", u"目标选择器参数分隔符", u',');
    static std::unique_ptr<NodeBase> nodeEqual = std::make_unique<NodeText>(
            u"TARGET_SELECTOR_ARGUMENT_EQUAu", u"等于",
            NormalId::make(u"=", u"等于"),
            [](const NodeBase *node, TokenReader &tokenReader) -> ASTNode {
                return tokenReader.readSymbolASTNode(node);
            });
    static std::unique_ptr<NodeBase> nodeNotEqual = std::make_unique<NodeText>(
            u"TARGET_SELECTOR_ARGUMENT_NOT_EQUAu", u"不等于",
            NormalId::make(u"=!", u"不等于"),
            [](const NodeBase *node, TokenReader &tokenReader) -> ASTNode {
                tokenReader.push();
                auto childNodes = {tokenReader.readSymbolASTNode(node), tokenReader.readSymbolASTNode(node)};
                return ASTNode::andNode(node, childNodes, tokenReader.collect());
            });
    static std::unique_ptr<NodeBase> nodeEqualOrNotEqual = std::make_unique<NodeOr>(
            u"TARGET_SELECTOR_ARGUMENT_SEPARATOR", u"目标选择器参数分隔符",
            std::vector<const NodeBase *>{
                    nodeEqual.get(), nodeNotEqual.get()},
            false);
    static std::unique_ptr<NodeBase> nodeRelativeFloat = std::make_unique<NodeRelativeFloat>(
            u"TARGET_SELECTOR_ARGUMENT_RELATIVE_FLOAT", u"目标选择器参数(小数)", false);
    static std::unique_ptr<NodeBase> nodeInteger = NodeInteger::make(
            u"TARGET_SELECTOR_ARGUMENT_INTEGER", u"目标选择器参数(整数)", std::nullopt, std::nullopt);
    static std::unique_ptr<NodeBase> nodeFloat = NodeFloat::make(
            u"TARGET_SELECTOR_ARGUMENT_FLOAT", u"目标选择器参数(小数)", std::nullopt, std::nullopt);
    static std::unique_ptr<NodeBase> nodeString = std::make_unique<NodeString>(
            u"TARGET_SELECTOR_ARGUMENT_STRING", u"目标选择器参数(字符串)", true, true, false);
    static std::unique_ptr<NodeBase> nodeScoreValueLeft = std::make_unique<NodeSingleSymbol>(
            u"TARGET_SELECTOR_ARGUMENT_SCORES_LEFT", u"目标选择器scores参数左括号", u'{');
    static std::unique_ptr<NodeBase> nodeScoreValueRight = std::make_unique<NodeSingleSymbol>(
            u"TARGET_SELECTOR_ARGUMENT_SCORES_RIGHT", u"目标选择器scores参数右括号", u'}');
    static std::unique_ptr<NodeBase> nodeScoreValueSeparator = std::make_unique<NodeSingleSymbol>(
            u"TARGET_SELECTOR_ARGUMENT_SCORES_SEPARATOR", u"目标选择器scores参数分隔符", u',');
    static std::unique_ptr<NodeBase> nodeScoreRange = std::make_unique<NodeRange>(
            u"TARGET_SELECTOR_ARGUMENT_SCORES_RANGE", u"目标选择器scores参数值的分数范围)");
    static std::unique_ptr<NodeBase> nodeScoreValueElement = std::make_unique<NodeEntry>(
            u"TARGET_SELECTOR_ARGUMENT_SCORES_ELEMENT", u"目标选择器scores参数内容",
            nodeString.get(), nodeEqualOrNotEqual.get(), nodeScoreRange.get());
    static std::unique_ptr<NodeBase> nodeScore = std::make_unique<NodeList>(
            u"TARGET_SELECTOR_ARGUMENT_SCORES", u"目标选择器scores参数",
            nodeScoreValueLeft.get(), nodeScoreValueElement.get(),
            nodeScoreValueSeparator.get(), nodeScoreValueRight.get());
    static std::unique_ptr<NodeBase> nodeHasItemValueLeft1 = std::make_unique<NodeSingleSymbol>(
            u"TARGET_SELECTOR_ARGUMENT_HASITEM_LEFT1", u"目标选择器hasitem参数左括号", u'{');
    static std::unique_ptr<NodeBase> nodeHasItemValueRight1 = std::make_unique<NodeSingleSymbol>(
            u"TARGET_SELECTOR_ARGUMENT_HASITEM_RIGHT1", u"目标选择器hasitem参数右括号", u'}');
    static std::unique_ptr<NodeBase> nodeHasItemValueSeparator = std::make_unique<NodeSingleSymbol>(
            u"TARGET_SELECTOR_ARGUMENT_HASITEM_SEPARATOR", u"目标选择器hasitem参数分隔符", u',');
    static std::unique_ptr<NodeBase> nodeHasItemValueLeft2 = std::make_unique<NodeSingleSymbol>(
            u"TARGET_SELECTOR_ARGUMENT_HASITEM_LEFT2", u"目标选择器hasitem参数左括号", u'[');
    static std::unique_ptr<NodeBase> nodeHasItemValueRight2 = std::make_unique<NodeSingleSymbol>(
            u"TARGET_SELECTOR_ARGUMENT_HASITEM_RIGHT2", u"目标选择器haspermission参数右括号", u']');
    static std::unique_ptr<NodeBase> nodeHasPermissionValueLeft = std::make_unique<NodeSingleSymbol>(
            u"TARGET_SELECTOR_ARGUMENT_HASPERMISSION_LEFT", u"目标选择器haspermission参数左括号", u'{');
    static std::unique_ptr<NodeBase> nodeHasPermissionValueRight = std::make_unique<NodeSingleSymbol>(
            u"TARGET_SELECTOR_ARGUMENT_HASPERMISSION_RIGHT", u"目标选择器haspermission参数右括号", u'}');
    static std::unique_ptr<NodeBase> nodeHasPermissionValueSeparator = std::make_unique<NodeSingleSymbol>(
            u"TARGET_SELECTOR_ARGUMENT_HASPERMISSION_SEPARATOR", u"目标选择器haspermission参数分隔符", u',');
    static std::unique_ptr<NodeBase> nodeHasPermissionState = std::make_unique<NodeNormalId>(
            u"PERMISSION_STATUS", u"权限状态", false,
            std::make_shared<std::vector<std::shared_ptr<NormalId>>>(std::vector<std::shared_ptr<NormalId>>{
                    NormalId::make(u"enabled", u"启用"),
                    NormalId::make(u"disabled", u"禁用")}));
    static std::unique_ptr<NodeBase> nodeHasPermissionEntry = std::make_unique<NodeEqualEntry>(
            u"TARGET_SELECTOR_ARGUMENT_HASPERMISSION_ENTRY", u"目标选择器haspermission参数内容",
            std::vector<EqualData>{
                    {u"camera", u"玩家能否转动相机视角", false, nodeHasPermissionState.get()},
                    {u"movement", u"玩家能否移动", false, nodeHasPermissionState.get()}});
    static std::unique_ptr<NodeBase> nodeHasPermission = std::make_unique<NodeList>(
            u"TARGET_SELECTOR_ARGUMENT_HASPERMISSION", u"目标选择器haspermission参数",
            nodeHasPermissionValueLeft.get(), nodeHasPermissionEntry.get(),
            nodeHasPermissionValueSeparator.get(), nodeHasPermissionValueRight.get());
    static std::unique_ptr<NodeBase> nodeHasPropertyValueLeft = std::make_unique<NodeSingleSymbol>(
            u"TARGET_SELECTOR_ARGUMENT_HASPROPERTY_LEFT", u"目标选择器has_property参数左括号", '{');
    static std::unique_ptr<NodeBase> nodeHasPropertyValueRight = std::make_unique<NodeSingleSymbol>(
            u"TARGET_SELECTOR_ARGUMENT_HASPROPERTY_RIGHT", u"目标选择器has_property参数右括号", '}');
    static std::unique_ptr<NodeBase> nodeHasPropertyValueSeparator = std::make_unique<NodeSingleSymbol>(
            u"TARGET_SELECTOR_ARGUMENT_HASPROPERTY_SEPARATOR", u"目标选择器has_property参数分隔符", ',');
    static std::unique_ptr<NodeBase> nodeBoolean = NodeBoolean::make(
            u"BOOLEAN", u"布尔值", std::nullopt, std::nullopt);
    static std::unique_ptr<NodeBase> nodeAny = std::make_unique<NodeOr>(
            u"ANY", u"任何值",
            std::vector<const NodeBase *>{
                    nodeBoolean.get(), nodeScoreRange.get(), nodeString.get(), nodeFloat.get()},
            false);
    static std::unique_ptr<NodeBase> nodeHasPropertyEntry = std::make_unique<NodeEntry>(
            u"TARGET_SELECTOR_ARGUMENT_HASPROPERTY_ENTRY", u"目标选择器has_property参数内容",
            nodeString.get(), nodeEqualOrNotEqual.get(), nodeAny.get());
    static std::unique_ptr<NodeBase> nodeHasProperty = std::make_unique<NodeList>(
            u"TARGET_SELECTOR_ARGUMENT_HASPERMISSION", u"目标选择器has_property参数",
            nodeHasPropertyValueLeft.get(), nodeHasPropertyEntry.get(),
            nodeHasPropertyValueSeparator.get(), nodeHasPropertyValueRight.get());
    static std::unique_ptr<NodeBase> nodeHasItemData = NodeInteger::make(
            u"TARGET_SELECTOR_ARGUMENT_HASITEM_ITEM_DATA", u"目标选择器hasitem参数物品数据值", 0, 32767);
    static std::unique_ptr<NodeBase> nodeHasItemQuantity = std::make_unique<NodeRange>(
            u"TARGET_SELECTOR_ARGUMENT_HASITEM_ITEM_QUANTITY", u"目标选择器hasitem限制范围内的所有槽位中符合条件的物品的总和数量");
    static std::unique_ptr<NodeBase> nodeHasItemSlotRange = std::make_unique<NodeRange>(
            u"TARGET_SELECTOR_ARGUMENT_HASITEM_SLOT_SLOT_RANGE", u"目标选择器hasitem要检测的槽位范围");

    void NodeTargetSelector::init(const CPack &cpack) {
        nodeItem = std::make_unique<NodeNamespaceId>(u"ITEM_ID", u"物品ID", u"item", true),
        nodeFamily = std::make_unique<NodeNormalId>(u"FAMILIES", u"族", u"entityFamily", true);
        nodeGameMode = std::make_unique<NodeNormalId>(u"GAME_MODES", u"游戏模式", u"gameMode", true),
        nodeSlot = std::make_unique<NodeNormalId>(u"SLOT", u"物品栏", u"entitySlot", true),
        nodeEntities = std::make_unique<NodeNamespaceId>(u"ENTITIES", u"实体", u"entity", true),
        nodeHasItemElement = std::make_unique<NodeEqualEntry>(
                u"TARGET_SELECTOR_ARGUMENT_HASITEM_ELEMENT", u"目标选择器参数值(物品检测)的内容",
                std::vector<EqualData>{
                        {u"item", u"要检测的物品", false, nodeItem.get()},
                        {u"data", u"要检测的物品的数据值", false, nodeHasItemData.get()},
                        {u"quantity", u"限制范围内的所有槽位中符合条件的物品的总和数量", false, nodeHasItemQuantity.get()},
                        {u"location", u"要检测的物品栏", false, nodeSlot.get()},
                        {u"slot", u"用于指定要检测的槽位范围", true, nodeHasItemSlotRange.get()}}),
        nodeHasItemList1 = std::make_unique<NodeList>(
                u"TARGET_SELECTOR_ARGUMENT_HASITEM", u"目标选择器参数值(物品检测)",
                nodeHasItemValueLeft1.get(), nodeHasItemElement.get(),
                nodeHasItemValueSeparator.get(), nodeHasItemValueRight1.get()),
        nodeHasItemList2 = std::make_unique<NodeList>(
                u"TARGET_SELECTOR_ARGUMENT_HASITEM", u"目标选择器参数值(物品检测)",
                nodeHasItemValueLeft2.get(), nodeHasItemList1.get(),
                nodeHasItemValueSeparator.get(), nodeHasItemValueRight2.get()),
        nodeHasItem = std::make_unique<NodeOr>(
                u"TARGET_SELECTOR_ARGUMENT_VALUE", u"目标选择器参数值(物品检测)",
                std::vector<const NodeBase *>{
                        nodeHasItemList1.get(), nodeHasItemList2.get()},
                false),
        nodeArgument = std::make_unique<NodeEqualEntry>(
                u"TARGET_SELECTOR_ARGUMENT", u"目标选择器单个参数",
                std::vector<EqualData>{
                        {u"x", u"x坐标", false, nodeRelativeFloat.get()},
                        {u"y", u"y坐标", false, nodeRelativeFloat.get()},
                        {u"z", u"z坐标", false, nodeRelativeFloat.get()},
                        {u"r", u"最大半径", false, nodeFloat.get()},
                        {u"rm", u"最小半径", false, nodeFloat.get()},
                        {u"dx", u"x坐标差异(检查实体的腿部位置)", false, nodeFloat.get()},
                        {u"dy", u"y坐标差异(检查实体的腿部位置)", false, nodeFloat.get()},
                        {u"dz", u"z坐标差异(检查实体的腿部位置)", false, nodeFloat.get()},
                        {u"scores", u"分数", false, nodeScore.get()},
                        {u"tag", u"标签", true, nodeString.get()},
                        {u"name", u"名字", true, nodeString.get()},
                        {u"type", u"实体类型", true, nodeEntities.get()},
                        {u"family", u"族", true, nodeFamily.get()},
                        {u"rx", u"最大垂直旋转", false, nodeFloat.get()},
                        {u"rxm", u"最小垂直旋转", false, nodeFloat.get()},
                        {u"ry", u"最大水平旋转", false, nodeFloat.get()},
                        {u"rym", u"最小水平旋转", false, nodeFloat.get()},
                        {u"hasitem", u"物品栏", false, nodeHasItem.get()},
                        {u"haspermission", u"权限", false, nodeHasPermission.get()},
                        {u"has_property", u"属性", false, nodeHasProperty.get()},
                        {u"l", u"最大经验等级", false, nodeFloat.get()},
                        {u"lm", u"最小经验等级", false, nodeFloat.get()},
                        {u"m", u"游戏模式", true, nodeGameMode.get()},
                        {u"c", u"目标数量(按照距离排序)", false, nodeInteger.get()},
                }),
        nodeArguments = std::make_unique<NodeList>(
                u"TARGET_SELECTOR_ARGUMENTS", u"目标选择器参数",
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

    NodeTypeId::NodeTypeId NodeTargetSelector::getNodeType() const {
        return NodeTypeId::TARGET_SELECTOR;
    }

    ASTNode NodeTargetSelector::getASTNode(TokenReader &tokenReader, const CPack *cpack) const {
        tokenReader.skipWhitespace();
        tokenReader.push();
        DEBUG_GET_NODE_BEGIN(nodeAt)
        ASTNode at = nodeAt->getASTNodeWithNextNode(tokenReader, cpack);
        DEBUG_GET_NODE_END(nodeAt)
        tokenReader.restore();
        if (HEDLEY_UNLIKELY(at.isError())) {
            //不是@符号开头
            if (isWildcard) {
                // 尝试匹配通配符
                tokenReader.push();
                DEBUG_GET_NODE_BEGIN(nodeWildcard)
                ASTNode wildcard = nodeWildcard->getASTNodeWithNextNode(tokenReader, cpack);
                DEBUG_GET_NODE_END(nodeWildcard)
                if (wildcard.isError()) {
                    tokenReader.restore();
                } else {
                    tokenReader.pop();
                    return wildcard;
                }
            }
            // 当作玩家名处理
            DEBUG_GET_NODE_BEGIN(nodePlayerName)
            ASTNode result = getByChildNode(tokenReader, cpack, nodePlayerName.get(), ASTNodeId::NODE_TARGET_SELECTOR_PLAYER_NAME);
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
                                    nullptr, ASTNodeId::NODE_TARGET_SELECTOR_NO_ARGUMENTS);
        }
        ASTNode arguments = nodeArguments->getASTNodeWithNextNode(tokenReader, cpack);
        return ASTNode::andNode(this, {targetSelectorVariable, arguments}, tokenReader.collect(),
                                nullptr, ASTNodeId::NODE_TARGET_SELECTOR_WITH_ARGUMENTS);
    }

    bool NodeTargetSelector::collectSuggestions(const ASTNode *astNode,
                                                size_t index,
                                                std::vector<Suggestions> &suggestions) const {
        if (HEDLEY_UNLIKELY(astNode->tokens.isEmpty())) {
            TokensView tokens = {astNode->tokens.lexerResult, astNode->tokens.end, astNode->tokens.end};
            ASTNode newAstNode = ASTNode::simpleNode(this, tokens);
            if (isWildcard) {
                nodeWildcard->collectSuggestions(astNode, index, suggestions);
            }
            nodeTargetSelectorVariable->collectSuggestions(astNode, index, suggestions);
            nodePlayerName->collectSuggestions(astNode, index, suggestions);
            return true;
        }
        if (HEDLEY_UNLIKELY(!astNode->isError() && astNode->id == ASTNodeId::NODE_TARGET_SELECTOR_NO_ARGUMENTS)) {
            TokensView tokens = {astNode->tokens.lexerResult, astNode->tokens.end, astNode->tokens.end};
            ASTNode newAstNode = ASTNode::simpleNode(this, tokens);
            nodeLeft->collectSuggestions(&newAstNode, index, suggestions);
        }
        return false;
    }

    void NodeTargetSelector::collectStructure(const ASTNode *astNode,
                                              StructureBuilder &structure,
                                              bool isMustHave) const {
        structure.append(isMustHave, u"目标选择器");
    }

}// namespace CHelper::Node