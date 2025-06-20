//
// Created by Yancey on 2024-05-25.
//

#include <chelper/node/param/NodeNormalId.h>
#include <chelper/node/param/NodeText.h>
#include <chelper/node/util/NodeEqualEntry.h>
#include <chelper/node/util/NodeOr.h>

namespace CHelper::Node {

    std::unique_ptr<NodeBase> NodeEqualEntry::nodeEqual = std::make_unique<NodeText>(
            "TARGET_SELECTOR_ARGUMENT_EQUAL", u"等于",
            NormalId::make(u"=", u"等于"),
            [](const NodeBase *node, TokenReader &tokenReader) -> ASTNode {
                return tokenReader.readSymbolASTNode(node);
            });
    std::unique_ptr<NodeBase> NodeEqualEntry::nodeNotEqual = std::make_unique<NodeText>(
            "TARGET_SELECTOR_ARGUMENT_NOT_EQUAL", u"不等于",
            NormalId::make(u"=!", u"不等于"),
            [](const NodeBase *node, TokenReader &tokenReader) -> ASTNode {
                tokenReader.push();
                auto childNodes = {tokenReader.readSymbolASTNode(node), tokenReader.readSymbolASTNode(node)};
                return ASTNode::andNode(node, childNodes, tokenReader.collect());
            });
    std::unique_ptr<NodeBase> NodeEqualEntry::nodeEqualOrNotEqual = std::make_unique<NodeOr>(
            std::vector<const NodeBase *>{
                    nodeEqual.get(), nodeNotEqual.get()},
            false);

    EqualData::EqualData(std::u16string name,
                         const std::optional<std::u16string> &description,
                         bool canUseNotEqual,
                         const NodeBase *nodeValue)
        : name(std::move(name)),
          description(description),
          canUseNotEqual(canUseNotEqual),
          nodeValue(nodeValue) {}

    NodeEqualEntry::NodeEqualEntry(std::vector<EqualData> equalDatas)
        : equalDatas(std::move(equalDatas)) {
        nodeKeyContent = std::make_shared<std::vector<std::shared_ptr<NormalId>>>();
        for (const auto &item: this->equalDatas) {
            nodeKeyContent->push_back(NormalId::make(item.name, item.description));
        }
        nodeKey = std::make_unique<NodeNormalId>("KEY", u"参数名", nodeKeyContent, true);
    }

    NodeTypeId::NodeTypeId NodeEqualEntry::getNodeType() const {
        return NodeTypeId::EQUAL_ENTRY;
    }

}// namespace CHelper::Node