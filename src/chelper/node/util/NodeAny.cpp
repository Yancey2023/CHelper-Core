//
// Created by Yancey on 2024-05-25.
//

#include "NodeAny.h"
#include "../param/NodeBoolean.h"
#include "../param/NodeRange.h"
#include "../param/NodeRelativeFloat.h"
#include "../param/NodeString.h"
#include "../param/NodeText.h"
#include "NodeEntry.h"
#include "NodeList.h"
#include "NodeSingleSymbol.h"

namespace CHelper::Node {

    static std::unique_ptr<NodeBase> nodeEqual = std::make_unique<NodeText>(
            u"TARGET_SELECTOR_ARGUMENT_EQUAu", u"等于",
            NormalId::make(u"=", u"等于"),
            [](const NodeBase *node, TokenReader &tokenReader) -> ASTNode {
                return tokenReader.readSymbolASTNode(node);
            });
    static std::unique_ptr<NodeBase> nodeNotEqual = std::make_unique<NodeText>(
            u"TARGET_SELECTOR_ARGUMENT_NOT_EQUAu", u"不等于",
            NormalId::make(u"=!", u"不等于"),
            [](const NodeBase *node, TokenReader &tokenReader) -> ASTNode {
                tokenReader.push();
                auto childNodes = {tokenReader.readSymbolASTNode(node), tokenReader.readSymbolASTNode(node)};
                return ASTNode::andNode(node, childNodes, tokenReader.collect());
            });
    static std::unique_ptr<NodeBase> nodeEqualOrNotEqual = std::make_unique<NodeOr>(
            u"TARGET_SELECTOR_ARGUMENT_SEPARATOR", u"等于或不等于",
            std::vector<const NodeBase *>{
                    nodeEqual.get(), nodeNotEqual.get()},
            false);
    static std::unique_ptr<NodeBase> nodeBoolean = std::make_unique<NodeBoolean>(
            u"BOOLEAN", u"布尔值", std::nullopt, std::nullopt);
    static std::unique_ptr<NodeBase> nodeRelativeFloat = std::make_unique<NodeRelativeFloat>(
            u"RELATIVE_FLOAT", u"坐标", true);
    static std::unique_ptr<NodeBase> nodeString = std::make_unique<NodeString>(
            u"STRING", u"字符串", true, true, false);
    static std::unique_ptr<NodeBase> nodeLeft1 = std::make_unique<NodeSingleSymbol>(
            u"LEFT1", u"左括号", u'{');
    static std::unique_ptr<NodeBase> nodeRight1 = std::make_unique<NodeSingleSymbol>(
            u"RIGHT1", u"右括号", u'}');
    static std::unique_ptr<NodeBase> nodeLeft2 = std::make_unique<NodeSingleSymbol>(
            u"LEFT12", u"左括号", u'[');
    static std::unique_ptr<NodeBase> nodeRight2 = std::make_unique<NodeSingleSymbol>(
            u"RIGHT2", u"右括号", u']');
    static std::unique_ptr<NodeBase> nodeSeparator = std::make_unique<NodeSingleSymbol>(
            u"SEPARATOR", u"分隔符", u',');
    static std::unique_ptr<NodeBase> nodeRange = std::make_unique<NodeRange>(
            u"RANGE", u"范围");
    static std::unique_ptr<NodeBase> nodeEntry = std::make_unique<NodeEntry>(
            u"ENTRY", u"参数内容",
            nodeString.get(), nodeEqualOrNotEqual.get(), NodeAny::getNodeAny());
    static std::unique_ptr<NodeBase> nodeObject = std::make_unique<NodeList>(
            u"OBJECT", u"对象",
            nodeLeft1.get(), nodeEntry.get(),
            nodeSeparator.get(), nodeRight1.get());
    static std::unique_ptr<NodeBase> nodeList = std::make_unique<NodeList>(
            u"OBJECT", u"对象",
            nodeLeft2.get(), NodeAny::getNodeAny(),
            nodeSeparator.get(), nodeRight2.get());

    NodeAny::NodeAny(const std::optional<std::u16string> &id,
                     const std::optional<std::u16string> &description)
        : NodeBase(id, description, false) {}

    NodeType *NodeAny::getNodeType() const {
        return NodeType::ANY.get();
    }

    void NodeAny::init(const CPack &cpack) {
        if (HEDLEY_UNLIKELY(node == nullptr)) {
            node = std::make_unique<NodeOr>(
                    u"VALUE", u"目标选择器参数值",
                    std::vector<const NodeBase *>{
                            nodeRelativeFloat.get(), nodeBoolean.get(),
                            nodeString.get(), nodeObject.get(),
                            nodeRange.get(), nodeList.get()},
                    false);
        }
    }

    ASTNode NodeAny::getASTNode(TokenReader &tokenReader, const CPack *cpack) const {
#if CHelperDebug == true
        if (HEDLEY_UNLIKELY(node == nullptr)) {
            throw std::runtime_error("not init");
        }
#endif
        return getByChildNode(tokenReader, cpack, node.get());
    }

    NodeAny *NodeAny::getNodeAny() {
        static std::unique_ptr<NodeAny> node = std::make_unique<NodeAny>(NodeAny(u"ANY", u"任何值"));
        return node.get();
    }

}// namespace CHelper::Node