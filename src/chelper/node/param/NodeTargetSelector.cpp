//
// Created by Yancey666 on 2023/12/1.
//

#include "NodeTargetSelector.h"
#include "NodeString.h"
#include "../util/NodeSingleSymbol.h"
#include "../util/NodeList.h"
#include "../selector/NodeTaregtSelectorArgument.h"

namespace CHelper::Node {

    std::shared_ptr<NodeBase> nodePlayerName = std::make_shared<NodeString>("PLAYER_NAME", "玩家名字", true, false);
    std::shared_ptr<NodeBase> nodeLeft = std::make_shared<NodeSingleSymbol>(
            "TARGET_SELECTOR_ARGUMENTS_LEFT", "目标选择器参数左括号", '[');
    std::shared_ptr<NodeBase> nodeRight = std::make_shared<NodeSingleSymbol>(
            "TARGET_SELECTOR_ARGUMENTS_RIGHT", "目标选择器参数右括号", '}');
    std::shared_ptr<NodeBase> nodeSeparator = std::make_shared<NodeSingleSymbol>(
            "TARGET_SELECTOR_ARGUMENTS_SEPARATOR", "目标选择器参数分隔符", ',');
    std::shared_ptr<NodeBase> nodeArgument = std::make_shared<NodeTargetSelectorArgument>(
            "TARGET_SELECTOR_ARGUMENTS_ARGUMENT", "目标选择器参数");
    std::shared_ptr<NodeList> nodeArguments = std::make_shared<NodeList>(
            "TARGET_SELECTOR_ARGUMENTS", "目标选择器参数",
            nodeLeft, nodeArgument, nodeSeparator, nodeRight);

    NODE_TYPE("TARGET_SELECTOR", NodeTargetSelector)

    NodeTargetSelector::NodeTargetSelector(const std::optional<std::string> &id,
                                           const std::optional<std::string> &description,
                                           const bool isMustPlayer,
                                           const bool isMustNPC,
                                           const bool isOnlyOne)
            : NodeBase(id, description),
              isMustPlayer(isMustPlayer),
              isMustNPC(isMustNPC),
              isOnlyOne(isOnlyOne),
              nodeTargetSelectorVariable("TARGET_SELECTOR_VARIABLE", "目标选择器变量",
                                         isMustPlayer, isMustNPC, isOnlyOne) {}

    NodeTargetSelector::NodeTargetSelector(const nlohmann::json &j,
                                           const CPack &cpack)
            : NodeBase(j, cpack),
              isMustPlayer(FROM_JSON(j, isMustPlayer, bool)),
              isMustNPC(FROM_JSON(j, isMustNPC, bool)),
              isOnlyOne(FROM_JSON(j, isOnlyOne, bool)),
              nodeTargetSelectorVariable("TARGET_SELECTOR_VARIABLE", "目标选择器变量",
                                         isMustPlayer, isMustNPC, isOnlyOne) {}

    void NodeTargetSelector::toJson(nlohmann::json &j) const {
        NodeBase::toJson(j);
        TO_JSON(j, isMustPlayer);
        TO_JSON(j, isMustNPC);
        TO_JSON(j, isOnlyOne);
    }

    ASTNode NodeTargetSelector::getASTNode(TokenReader &tokenReader, const CPack &cpack) const {
        tokenReader.push();
        ASTNode at = getSymbolASTNode(tokenReader, '@');
        if (at.isError()) {
            //不是@符号开头，当作玩家名处理
            tokenReader.restore();
            return getByChildNode(tokenReader, cpack, nodePlayerName);
        }
        //@符号开头，进入目标选择器检测
        //目标选择器变量
        ASTNode targetSelectorVariable = nodeTargetSelectorVariable.getASTNode(tokenReader, cpack);
        tokenReader.push();
        ASTNode lestBracket = getSymbolASTNode(tokenReader, '[');
        tokenReader.restore();
        if (lestBracket.isError()) {
            //没有后面的[...]
            return ASTNode::andNode(this, {{at, targetSelectorVariable}}, tokenReader.collect());
        }
        //TODO 目标选择器的解析
        return ASTNode::andNode(this, {{at, targetSelectorVariable}}, tokenReader.collect());
    }

} // CHelper::Node