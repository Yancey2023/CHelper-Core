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
            "TARGET_SELECTOR_ARGUMENT_EQUAL", "等于",
            NormalId::make("=", "等于"),
            [](const NodeBase *node, TokenReader &tokenReader) -> ASTNode {
                return tokenReader.readSymbolASTNode(node);
            });
    static std::unique_ptr<NodeBase> nodeNotEqual = std::make_unique<NodeText>(
            "TARGET_SELECTOR_ARGUMENT_NOT_EQUAL", "不等于",
            NormalId::make("=!", "不等于"),
            [](const NodeBase *node, TokenReader &tokenReader) -> ASTNode {
                tokenReader.push();
                auto childNodes = {tokenReader.readSymbolASTNode(node), tokenReader.readSymbolASTNode(node)};
                return ASTNode::andNode(node, childNodes, tokenReader.collect());
            });
    static std::unique_ptr<NodeBase> nodeEqualOrNotEqual = std::make_unique<NodeOr>(
            "TARGET_SELECTOR_ARGUMENT_SEPARATOR", "等于或不等于",
            std::vector<const NodeBase *>{
                    nodeEqual.get(), nodeNotEqual.get()},
            false);
    static std::unique_ptr<NodeBase> nodeBoolean = std::make_unique<NodeBoolean>(
            "BOOLEAN", "布尔值", std::nullopt, std::nullopt);
    static std::unique_ptr<NodeBase> nodeRelativeFloat = std::make_unique<NodeRelativeFloat>(
            "RELATIVE_FLOAT", "坐标", true);
    static std::unique_ptr<NodeBase> nodeString = std::make_unique<NodeString>(
            "STRING", "字符串", true, true, false);
    static std::unique_ptr<NodeBase> nodeLeft1 = std::make_unique<NodeSingleSymbol>(
            "LEFT1", "左括号", '{');
    static std::unique_ptr<NodeBase> nodeRight1 = std::make_unique<NodeSingleSymbol>(
            "RIGHT1", "右括号", '}');
    static std::unique_ptr<NodeBase> nodeLeft2 = std::make_unique<NodeSingleSymbol>(
            "LEFT12", "左括号", '[');
    static std::unique_ptr<NodeBase> nodeRight2 = std::make_unique<NodeSingleSymbol>(
            "RIGHT2", "右括号", ']');
    static std::unique_ptr<NodeBase> nodeSeparator = std::make_unique<NodeSingleSymbol>(
            "SEPARATOR", "分隔符", ',');
    static std::unique_ptr<NodeBase> nodeRange = std::make_unique<NodeRange>(
            "RANGE", "范围");
    static std::unique_ptr<NodeBase> nodeEntry = std::make_unique<NodeEntry>(
            "ENTRY", "参数内容",
            nodeString.get(), nodeEqualOrNotEqual.get(), NodeAny::getNodeAny());
    static std::unique_ptr<NodeBase> nodeObject = std::make_unique<NodeList>(
            "OBJECT", "对象",
            nodeLeft1.get(), nodeEntry.get(),
            nodeSeparator.get(), nodeRight1.get());
    static std::unique_ptr<NodeBase> nodeList = std::make_unique<NodeList>(
            "OBJECT", "对象",
            nodeLeft2.get(), NodeAny::getNodeAny(),
            nodeSeparator.get(), nodeRight2.get());

    NodeAny::NodeAny(const std::optional<std::string> &id,
                     const std::optional<std::string> &description)
        : NodeBase(id, description, false) {}

    void NodeAny::init(const CPack &cpack) {
        if (node == nullptr) {
            node = std::make_unique<NodeOr>(
                    "VALUE", "目标选择器参数值",
                    std::vector<const NodeBase *>{
                            nodeRelativeFloat.get(), nodeBoolean.get(),
                            nodeString.get(), nodeObject.get(),
                            nodeRange.get(), nodeList.get()},
                    false);
        }
    }

    ASTNode NodeAny::getASTNode(TokenReader &tokenReader, const CPack *cpack) const {
#if CHelperDebug == true
        if (node == nullptr) {
            throw std::runtime_error("not init");
        }
#endif
        return getByChildNode(tokenReader, cpack, node.get());
    }

    NodeAny *NodeAny::getNodeAny() {
        static std::unique_ptr<NodeAny> node = std::make_unique<NodeAny>(NodeAny("ANY", "任何值"));
        return node.get();
    }

}// namespace CHelper::Node