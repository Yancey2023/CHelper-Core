//
// Created by Yancey on 2023/12/1.
//

#include "NodeTargetSelector.h"
#include "NodeString.h"
#include "../util/NodeSingleSymbol.h"
#include "../util/NodeList.h"
#include "../selector/NodeTargetSelectorArgument.h"
#include "../util/NodeOr.h"
#include "../util/NodeAnd.h"
#include "NodeNormalId.h"

namespace CHelper::Node {

    std::shared_ptr<NodeBase> nodePlayerName = std::make_shared<NodeString>(
            "TARGET_SELECTOR_PLAYER_NAME", "玩家名字", true, false);
    std::shared_ptr<NodeBase> nodeAt = std::make_shared<NodeSingleSymbol>(
            "TARGET_SELECTOR_AT", "@符号", '@');
    std::shared_ptr<NodeBase> nodeTargetSelectorVariable = std::make_shared<NodeNormalId>(
            "TARGET_SELECTOR_VARIABLE", "目标选择器变量", std::nullopt,
            std::make_shared<std::vector<std::shared_ptr<NormalId>>>(std::vector<std::shared_ptr<NormalId>>{
                    std::make_shared<NormalId>("@e", "选择所有实体(只选择活着的实体)"),
                    std::make_shared<NormalId>("@a", "选择所有玩家(无论死活)"),
                    std::make_shared<NormalId>("@r", "选择一名随机玩家(可通过type选择非玩家实体)(只选择活着的实体)"),
                    std::make_shared<NormalId>("@p", "选择最近的玩家(若距离相同，会在其中选择最晚进入服务器的玩家)"),
                    std::make_shared<NormalId>("@s", "命令的执行者(只选择唯一一个实体)(包括已死亡玩家)"),
                    std::make_shared<NormalId>("@initiator", "选择当前与该NPC进行交互(在NPC内置的命令界面中使用)")
            }), [](const NodeBase *node, TokenReader &tokenReader) -> ASTNode {
                tokenReader.push();
                auto childNodes = {tokenReader.getSymbolASTNode(node), tokenReader.getStringASTNode(node)};
                return ASTNode::andNode(node, childNodes, tokenReader.collect());
            }
    );
    std::shared_ptr<NodeBase> nodeLeft = std::make_shared<NodeSingleSymbol>(
            "TARGET_SELECTOR_ARGUMENTS_LEFT", "目标选择器参数左括号", '[');
    std::shared_ptr<NodeBase> nodeRight = std::make_shared<NodeSingleSymbol>(
            "TARGET_SELECTOR_ARGUMENTS_RIGHT", "目标选择器参数右括号", ']');
    std::shared_ptr<NodeBase> nodeSeparator = std::make_shared<NodeSingleSymbol>(
            "TARGET_SELECTOR_ARGUMENTS_SEPARATOR", "目标选择器参数分隔符", ',');
    std::shared_ptr<NodeBase> nodeArgument = std::make_shared<NodeTargetSelectorArgument>(
            "TARGET_SELECTOR_ARGUMENTS_ARGUMENT", "目标选择器单个参数");
    std::shared_ptr<NodeList> nodeArguments = std::make_shared<NodeList>(
            "TARGET_SELECTOR_ARGUMENTS", "目标选择器参数",
            nodeLeft, nodeArgument, nodeSeparator, nodeRight);

    NodeTargetSelector::NodeTargetSelector(const std::optional<std::string> &id,
                                           const std::optional<std::string> &description,
                                           const bool isMustPlayer,
                                           const bool isMustNPC,
                                           const bool isOnlyOne)
            : NodeBase(id, description, false),
              isMustPlayer(isMustPlayer),
              isMustNPC(isMustNPC),
              isOnlyOne(isOnlyOne) {}

    NodeTargetSelector::NodeTargetSelector(const nlohmann::json &j,
                                           [[maybe_unused]] const CPack &cpack)
            : NodeBase(j),
              isMustPlayer(FROM_JSON(j, isMustPlayer, bool)),
              isMustNPC(FROM_JSON(j, isMustNPC, bool)),
              isOnlyOne(FROM_JSON(j, isOnlyOne, bool)) {}

    NodeType NodeTargetSelector::getNodeType() const {
        return NodeType::TARGET_SELECTOR;
    }

    void NodeTargetSelector::toJson(nlohmann::json &j) const {
        NodeBase::toJson(j);
        TO_JSON(j, isMustPlayer);
        TO_JSON(j, isMustNPC);
        TO_JSON(j, isOnlyOne);
    }

    ASTNode NodeTargetSelector::getASTNode(TokenReader &tokenReader) const {
        tokenReader.skipWhitespace();
        tokenReader.push();
        ASTNode at = nodeAt->getASTNode(tokenReader);
        tokenReader.restore();
        if (at.isError()) {
            //不是@符号开头，当作玩家名处理
            return getByChildNode(tokenReader, nodePlayerName, "target selector");
        }
        //@符号开头，进入目标选择器检测
        //目标选择器变量
        ASTNode targetSelectorVariable = nodeTargetSelectorVariable->getASTNode(tokenReader);
        tokenReader.push();
        ASTNode leftBracket = nodeLeft->getASTNode(tokenReader);
        tokenReader.restore();
        if (leftBracket.isError()) {
            //没有后面的[...]
            return ASTNode::andNode(this, {targetSelectorVariable}, tokenReader.collect(),
                                    nullptr, "target selector");
        }
        //TODO 目标选择器的解析
        return ASTNode::andNode(this, {targetSelectorVariable}, tokenReader.collect(),
                                nullptr, "target selector");
    }

    bool NodeTargetSelector::collectSuggestions(const ASTNode *astNode,
                                                size_t index,
                                                std::vector<Suggestion> &suggestions) const {
        if (!astNode->tokens.isEmpty()) {
            return false;
        }
        nodeAt->collectSuggestions(astNode, index, suggestions);
        nodePlayerName->collectSuggestions(astNode, index, suggestions);
        return true;
    }

    void NodeTargetSelector::collectStructure(const ASTNode *astNode,
                                              StructureBuilder &structure,
                                              bool isMustHave) const {
        if (astNode == nullptr || astNode->id == "target selector") {
            structure.append(isMustHave, "目标选择器");
        }
    }

} // CHelper::Node