//
// Created by Yancey on 2024/3/1.
//

#include "NodeXpInteger.h"
#include "../util/NodeAnd.h"
#include "../util/NodeOr.h"
#include "NodeInteger.h"
#include "NodeText.h"

namespace CHelper::Node {

    static std::shared_ptr<NodeBase> nodeInteger = std::make_shared<NodeInteger>(
            L"INTEGER", L"整数", std::nullopt, std::nullopt);
    static std::shared_ptr<NodeBase> nodeLevel = std::make_shared<NodeText>(
            L"L", L"等级", NormalId::make(L"L", L"等级"));
    static std::shared_ptr<NodeBase> levelXp = std::make_shared<NodeAnd>(
            L"LEVEL_XP", L"等级经验", WhitespaceMode::NO_WHITESPACE,
            std::vector<const NodeBase *>{nodeInteger.get(), nodeLevel.get()});
    static std::shared_ptr<NodeBase> xp = std::make_shared<NodeOr>(
            L"XP", L"经验",
            std::vector<const NodeBase *>{levelXp.get(), nodeInteger.get()},
            false, true);

    NodeType *NodeXpInteger::getNodeType() const {
        return NodeType::XP_INTEGER.get();
    }

    ASTNode NodeXpInteger::getASTNode(TokenReader &tokenReader, const CPack *cpack) const {
        return getByChildNode(tokenReader, cpack, xp.get());
    }

    void NodeXpInteger::collectStructure(const ASTNode *astNode,
                                         StructureBuilder &structure,
                                         bool isMustHave) const {
        structure.append(isMustHave, description.value_or(L"经验值"));
    }

    bool NodeXpInteger::collectColor(const ASTNode *astNode,
                                 ColoredString &coloredString,
                                 const Theme &theme) const {
        coloredString.setColor(astNode->tokens, theme.colorInteger);
        return true;
    }

    CODEC_UNIQUE_PTR(NodeXpInteger)

}// namespace CHelper::Node