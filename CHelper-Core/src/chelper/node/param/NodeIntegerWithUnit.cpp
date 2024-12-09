//
// Created by Yancey on 2024/3/1.
//

#include <chelper/node/param/NodeIntegerWithUnit.h>
#include <chelper/node/template/NodeTemplateNumber.h>
#include <chelper/node/util/NodeAnd.h>
#include <chelper/node/util/NodeOr.h>

namespace CHelper::Node {

    static std::shared_ptr<NodeBase> nodeInteger = NodeInteger::make(
            u"INTEGER", u"整数", std::nullopt, std::nullopt);

    void NodeIntegerWithUnit::init(const CPack &cpack) {
        nodeUnits = std::make_unique<NodeNormalId>(u"UNITS", u"单位", false, units);
        nodeIntegerWithUnit = std::make_unique<NodeAnd>(
                u"NODE_INTEGER_WITH_UINT", u"带单位的整数", WhitespaceMode::NO_WHITESPACE,
                std::vector<const NodeBase *>{nodeInteger.get(), nodeUnits.get()});
        nodeIntegerMaybeHaveUnit = std::make_unique<NodeOr>(
                u"NODE_INTEGER_MAYBE_HAVE_UNIT", u"可能带单位的整数",
                std::vector<const NodeBase *>{nodeIntegerWithUnit.get(), nodeInteger.get()},
                false, true);
    }

    NodeTypeId::NodeTypeId NodeIntegerWithUnit::getNodeType() const {
        return NodeTypeId::INTEGER_WITH_UNIT;
    }

    ASTNode NodeIntegerWithUnit::getASTNode(TokenReader &tokenReader, const CPack *cpack) const {
        return getByChildNode(tokenReader, cpack, nodeIntegerMaybeHaveUnit.get());
    }

    void NodeIntegerWithUnit::collectStructure(const ASTNode *astNode,
                                               StructureBuilder &structure,
                                               bool isMustHave) const {
        structure.append(isMustHave, description.value_or(u"可能带单位的整数"));
    }

    bool NodeIntegerWithUnit::collectColor(const ASTNode *astNode,
                                           ColoredString &coloredString,
                                           const Theme &theme) const {
        coloredString.setColor(astNode->tokens, theme.colorInteger);
        return true;
    }

}// namespace CHelper::Node