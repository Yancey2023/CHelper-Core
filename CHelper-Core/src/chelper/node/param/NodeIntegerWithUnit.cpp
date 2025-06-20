//
// Created by Yancey on 2024/3/1.
//

#include <chelper/node/param/NodeIntegerWithUnit.h>
#include <chelper/node/template/NodeTemplateNumber.h>
#include <chelper/node/util/NodeAnd.h>
#include <chelper/node/util/NodeOr.h>

namespace CHelper::Node {

    static std::shared_ptr<NodeBase> nodeInteger = NodeInteger::make(
            "INTEGER", u"整数", std::nullopt, std::nullopt);

    void NodeIntegerWithUnit::init(const CPack &cpack) {
        nodeUnits = std::make_unique<NodeNormalId>("UNITS", u"单位", units, false);
        nodeIntegerWithUnit = std::make_unique<NodeAnd>(
                std::vector<const NodeBase *>{nodeInteger.get(), nodeUnits.get()});
        nodeIntegerMaybeHaveUnit = std::make_unique<NodeOr>(
                std::vector<const NodeBase *>{nodeIntegerWithUnit.get(), nodeInteger.get()},
                false, true);
    }

    NodeTypeId::NodeTypeId NodeIntegerWithUnit::getNodeType() const {
        return NodeTypeId::INTEGER_WITH_UNIT;
    }

    ASTNode NodeIntegerWithUnit::getASTNode(TokenReader &tokenReader, const CPack *cpack) const {
        return getByChildNode(tokenReader, cpack, nodeIntegerMaybeHaveUnit.get());
    }

}// namespace CHelper::Node