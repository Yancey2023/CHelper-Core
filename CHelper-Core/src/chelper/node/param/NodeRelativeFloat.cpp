//
// Created by Yancey on 2023/12/20.
//

#include <chelper/node/param/NodeRelativeFloat.h>
#include <chelper/node/util/NodeOr.h>
#include <chelper/node/util/NodeSingleSymbol.h>

namespace CHelper::Node {

    std::unique_ptr<NodeSingleSymbol> NodeRelativeFloat::nodeRelativeNotation = std::make_unique<NodeSingleSymbol>(
            u'~', u"相对坐标（~x ~y ~z）", false);
    std::unique_ptr<NodeSingleSymbol> NodeRelativeFloat::nodeCaretNotation = std::make_unique<NodeSingleSymbol>(
            u'^', u"局部坐标（^左 ^上 ^前）", false);
    std::unique_ptr<NodeOr> NodeRelativeFloat::nodePreSymbol = std::make_unique<NodeOr>(
            std::vector<const NodeBase *>{nodeRelativeNotation.get(), nodeCaretNotation.get()}, false);

    NodeRelativeFloat::NodeRelativeFloat(const std::optional<std::string> &id,
                                         const std::optional<std::u16string> &description,
                                         bool canUseCaretNotation)
        : NodeSerializable(id, description, false),
          canUseCaretNotation(canUseCaretNotation) {}

    NodeTypeId::NodeTypeId NodeRelativeFloat::getNodeType() const {
        return NodeTypeId::RELATIVE_FLOAT;
    }

}// namespace CHelper::Node