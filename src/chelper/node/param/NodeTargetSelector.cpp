//
// Created by Yancey on 2023/12/1.
//

#include "NodeTargetSelector.h"
#include "../util/NodeSingleSymbol.h"
#include "NodeItem.h"
#include "NodeNormalId.h"
#include "NodeString.h"

namespace CHelper::Node {

    static std::shared_ptr<NodeBase> nodePlayerName = std::make_shared<NodeString>(
            "TARGET_SELECTOR_PLAYER_NAME", "玩家名字", false, true, false);
    static std::shared_ptr<NodeBase> nodeAt = std::make_shared<NodeSingleSymbol>(
            "TARGET_SELECTOR_AT", "@符号", '@');
    static std::shared_ptr<NodeBase> nodeTargetSelectorVariable = std::make_shared<NodeNormalId>(
            "TARGET_SELECTOR_VARIABLE", "目标选择器变量", std::nullopt, true,
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

    NodeTargetSelector::NodeTargetSelector(const std::optional<std::string> &id,
                                           const std::optional<std::string> &description,
                                           const bool isMustPlayer,
                                           const bool isMustNPC,
                                           const bool isOnlyOne,
                                           const NodeNamespaceId &nodeItem,
                                           const NodeNormalId &nodeFamily,
                                           const NodeNormalId &nodeGameMode,
                                           const NodeNormalId &nodeItemLocation,
                                           const NodeNamespaceId &nodeEntities)
        : NodeBase(id, description, false),
          isMustPlayer(isMustPlayer),
          isMustNPC(isMustNPC),
          isOnlyOne(isOnlyOne),
          nodeItem(nodeItem),
          nodeFamily(nodeFamily),
          nodeGameMode(nodeGameMode),
          nodeItemLocation(nodeItemLocation),
          nodeEntities(nodeEntities),
          nodeArgument("TARGET_SELECTOR_ARGUMENT", "目标选择器单个参数",
                       &nodeItem, &nodeFamily, &nodeGameMode, &nodeItemLocation, &nodeEntities),
          nodeArguments("TARGET_SELECTOR_ARGUMENTS", "目标选择器参数",
                        nodeLeft.get(), &nodeArgument, nodeSeparator.get(), nodeRight.get()) {}

    NodeTargetSelector::NodeTargetSelector(const nlohmann::json &j,
                                           [[maybe_unused]] const CPack &cpack)
        : NodeBase(j, true),
          isMustPlayer(JsonUtil::read<bool>(j, "isMustPlayer")),
          isMustNPC(JsonUtil::read<bool>(j, "isMustNPC")),
          isOnlyOne(JsonUtil::read<bool>(j, "isOnlyOne")),
          nodeItem("ITEM_ID", "物品ID", "items", true, cpack.itemIds),
          nodeFamily("FAMILIES", "族", "families", true, cpack.getNormalId("families")),
          nodeGameMode("GAME_MODES", "游戏模式", "gameModes", true, cpack.getNormalId("gameModes")),
          nodeItemLocation("SLOT", "物品栏", "slot", true, cpack.getNormalId("slot")),
          nodeEntities("ENTITIES", "实体", "entities", true, cpack.getNamespaceId("entities")),
          nodeArgument("TARGET_SELECTOR_ARGUMENT", "目标选择器单个参数",
                       &nodeItem, &nodeFamily, &nodeGameMode, &nodeItemLocation, &nodeEntities),
          nodeArguments("TARGET_SELECTOR_ARGUMENTS", "目标选择器参数",
                        nodeLeft.get(), &nodeArgument, nodeSeparator.get(), nodeRight.get()) {}

    NodeTargetSelector::NodeTargetSelector(BinaryReader &binaryReader,
                                           [[maybe_unused]] const CPack &cpack)
        : NodeBase(binaryReader),
          nodeItem("ITEM_ID", "物品ID", "items", true, cpack.itemIds),
          nodeFamily("FAMILIES", "族", "families", true, cpack.getNormalId("families")),
          nodeGameMode("GAME_MODES", "游戏模式", "gameModes", true, cpack.getNormalId("gameModes")),
          nodeItemLocation("SLOT", "物品栏", "slot", true, cpack.getNormalId("slot")),
          nodeEntities("ENTITIES", "实体", "entities", true, cpack.getNamespaceId("entities")),
          nodeArgument("TARGET_SELECTOR_ARGUMENT", "目标选择器单个参数",
                       &nodeItem, &nodeFamily, &nodeGameMode, &nodeItemLocation, &nodeEntities),
          nodeArguments("TARGET_SELECTOR_ARGUMENTS", "目标选择器参数",
                        nodeLeft.get(), &nodeArgument, nodeSeparator.get(), nodeRight.get()) {
        isMustPlayer = binaryReader.read<bool>();
        isMustNPC = binaryReader.read<bool>();
        isOnlyOne = binaryReader.read<bool>();
    }

    NodeType *NodeTargetSelector::getNodeType() const {
        return NodeType::TARGET_SELECTOR.get();
    }

    void NodeTargetSelector::toJson(nlohmann::json &j) const {
        NodeBase::toJson(j);
        JsonUtil::encode(j, "isMustPlayer", isMustPlayer);
        JsonUtil::encode(j, "isMustNPC", isMustNPC);
        JsonUtil::encode(j, "isOnlyOne", isOnlyOne);
    }

    void NodeTargetSelector::writeBinToFile(BinaryWriter &binaryWriter) const {
        NodeBase::writeBinToFile(binaryWriter);
        binaryWriter.encode(isMustPlayer);
        binaryWriter.encode(isMustNPC);
        binaryWriter.encode(isOnlyOne);
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
                                    nullptr, "target selector no arguments", false);
        }
        ASTNode arguments = nodeArguments.getASTNodeWithNextNode(tokenReader, cpack);
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

}// namespace CHelper::Node