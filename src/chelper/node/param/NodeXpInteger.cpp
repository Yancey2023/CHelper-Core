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
            "INTEGER", "整数", std::nullopt, std::nullopt);
    static std::shared_ptr<NodeBase> nodeLevel = std::make_shared<NodeText>(
            "L", "等级", NormalId::make("L", "等级"));
    static std::shared_ptr<NodeBase> levelXp = std::make_shared<NodeAnd>(
            "LEVEL_XP", "等级经验", WhitespaceMode::NO_WHITESPACE,
            std::vector<const NodeBase *>{nodeInteger.get(), nodeLevel.get()});
    static std::shared_ptr<NodeBase> xp = std::make_shared<NodeOr>(
            "XP", "经验",
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
        structure.append(isMustHave, description.value_or("经验值"));
    }

    CODEC_UNIQUE_PTR(NodeXpInteger)

}// namespace CHelper::Node