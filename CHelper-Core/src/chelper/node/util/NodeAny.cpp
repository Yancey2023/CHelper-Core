//
// Created by Yancey on 2024-05-25.
//

#include <chelper/node/param/NodeRange.h>
#include <chelper/node/param/NodeRelativeFloat.h>
#include <chelper/node/param/NodeString.h>
#include <chelper/node/param/NodeText.h>
#include <chelper/node/template/NodeTemplateBoolean.h>
#include <chelper/node/util/NodeAny.h>
#include <chelper/node/util/NodeEntry.h>
#include <chelper/node/util/NodeList.h>
#include <chelper/node/util/NodeSingleSymbol.h>

namespace CHelper::Node {

    static std::unique_ptr<NodeBase> nodeEqual = std::make_unique<NodeText>(
            "TARGET_SELECTOR_ARGUMENT_EQUAL", u"等于",
            NormalId::make(u"=", u"等于"),
            [](const NodeBase *node, TokenReader &tokenReader) -> ASTNode {
                return tokenReader.readSymbolASTNode(node);
            });
    static std::unique_ptr<NodeBase> nodeNotEqual = std::make_unique<NodeText>(
            "TARGET_SELECTOR_ARGUMENT_NOT_EQUAL", u"不等于",
            NormalId::make(u"=!", u"不等于"),
            [](const NodeBase *node, TokenReader &tokenReader) -> ASTNode {
                tokenReader.push();
                auto childNodes = {tokenReader.readSymbolASTNode(node), tokenReader.readSymbolASTNode(node)};
                return ASTNode::andNode(node, childNodes, tokenReader.collect());
            });
    static std::unique_ptr<NodeBase> nodeEqualOrNotEqual = std::make_unique<NodeOr>(
            std::vector<const NodeBase *>{
                    nodeEqual.get(), nodeNotEqual.get()},
            false);
    static std::unique_ptr<NodeBase> nodeBoolean = NodeBoolean::make(
            "BOOLEAN", u"布尔值", std::nullopt, std::nullopt);
    static std::unique_ptr<NodeBase> nodeRelativeFloat = std::make_unique<NodeRelativeFloat>(
            "RELATIVE_FLOAT", u"坐标", true);
    static std::unique_ptr<NodeBase> nodeString = std::make_unique<NodeString>(
            "STRING", u"字符串", true, true, false);
    static std::unique_ptr<NodeBase> nodeLeft1 = std::make_unique<NodeSingleSymbol>(u'{', u"左括号");
    static std::unique_ptr<NodeBase> nodeRight1 = std::make_unique<NodeSingleSymbol>(u'}', u"右括号");
    static std::unique_ptr<NodeBase> nodeLeft2 = std::make_unique<NodeSingleSymbol>(u'[', u"左括号");
    static std::unique_ptr<NodeBase> nodeRight2 = std::make_unique<NodeSingleSymbol>(u']', u"右括号");
    static std::unique_ptr<NodeBase> nodeSeparator = std::make_unique<NodeSingleSymbol>(u',', u"分隔符");
    static std::unique_ptr<NodeBase> nodeRange = std::make_unique<NodeRange>(
            "RANGE", u"范围");
    static std::unique_ptr<NodeBase> nodeEntry = std::make_unique<NodeEntry>(
            nodeString.get(), nodeEqualOrNotEqual.get(), NodeAny::getNodeAny());
    static std::unique_ptr<NodeBase> nodeObject = std::make_unique<NodeList>(
            nodeLeft1.get(), nodeEntry.get(),
            nodeSeparator.get(), nodeRight1.get());
    static std::unique_ptr<NodeBase> nodeList = std::make_unique<NodeList>(
            nodeLeft2.get(), NodeAny::getNodeAny(),
            nodeSeparator.get(), nodeRight2.get());

    NodeTypeId::NodeTypeId NodeAny::getNodeType() const {
        return NodeTypeId::ANY;
    }

    void NodeAny::init(const CPack &cpack) {
        if (HEDLEY_UNLIKELY(node == nullptr)) {
            node = std::make_unique<NodeOr>(
                    std::vector<const NodeBase *>{
                            nodeRelativeFloat.get(), nodeBoolean.get(),
                            nodeString.get(), nodeObject.get(),
                            nodeRange.get(), nodeList.get()},
                    false);
        }
    }

    NodeAny *NodeAny::getNodeAny() {
        static NodeAny node;
        return &node;
    }

}// namespace CHelper::Node