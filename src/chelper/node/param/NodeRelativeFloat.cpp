//
// Created by Yancey on 2023/12/20.
//

#include "NodeRelativeFloat.h"
#include "../../util/TokenUtil.h"
#include "../util/NodeSingleSymbol.h"
#include "NodeFloat.h"
#include "../util/NodeAnd.h"
#include "../util/NodeOr.h"

namespace CHelper::Node {

    static std::unique_ptr<NodeBase> nodeRelativeNotation = std::make_unique<NodeSingleSymbol>(
            "RELATIVE_FLOAT_RELATIVE_NOTATION", "相对坐标（~x ~y ~z）", '~');
    static std::unique_ptr<NodeBase> nodeCaretNotation = std::make_unique<NodeSingleSymbol>(
            "RELATIVE_FLOAT_CARET_NOTATION", "局部坐标（^左 ^上 ^右）", '^');
    static std::unique_ptr<NodeBase> nodeValue = std::make_unique<NodeFloat>(
            "RELATIVE_FLOAT_FLOAT", "坐标参数的数值", std::nullopt, std::nullopt);
    static std::unique_ptr<NodeBase> nodeRelative = std::make_unique<NodeAnd>(
            "RELATIVE_FLOAT_RELATIVE", "相对坐标（~x ~y ~z）",
            std::vector<const NodeBase *>{nodeRelativeNotation.get(), nodeValue.get()});
    static std::unique_ptr<NodeBase> nodeCaret = std::make_unique<NodeAnd>(
            "RELATIVE_FLOAT_CARET", "局部坐标（^左 ^上 ^右）",
            std::vector<const NodeBase *>{nodeCaretNotation.get(), nodeValue.get()});
    static std::unique_ptr<NodeBase> nodeRelativeFloat = std::make_unique<NodeOr>(
            "RELATIVE_FLOAT", "坐标", std::vector<const NodeBase *>{
                    nodeRelative.get(), nodeCaret.get(), nodeRelativeNotation.get(),
                    nodeCaretNotation.get(), nodeValue.get()}, false);

    NodeRelativeFloat::NodeRelativeFloat(const std::optional<std::string> &id,
                                         const std::optional<std::string> &description,
                                         bool canUseCaretNotation)
            : NodeBase(id, description, false),
              canUseCaretNotation(canUseCaretNotation) {}

    NodeRelativeFloat::NodeRelativeFloat(const nlohmann::json &j,
                                         [[maybe_unused]] const CPack &cpack)
            : NodeBase(j, true),
              canUseCaretNotation(JsonUtil::fromJson<bool>(j, "canUseCaretNotation")) {}

    NodeType *NodeRelativeFloat::getNodeType() const {
        return NodeType::RELATIVE_FLOAT.get();
    }

    void NodeRelativeFloat::toJson(nlohmann::json &j) const {
        NodeBase::toJson(j);
        JsonUtil::toJson(j, "canUseCaretNotation", canUseCaretNotation);
    }

    ASTNode NodeRelativeFloat::getASTNode(TokenReader &tokenReader, const CPack *cpack) const {
        return getASTNode(this, cpack, tokenReader, canUseCaretNotation).second;
    }

    std::pair<std::uint8_t, ASTNode> NodeRelativeFloat::getASTNode(const NodeBase *node,
                                                                   const CPack *cpack,
                                                                   TokenReader &tokenReader,
                                                                   bool canUseCaretNotation) {
        // 0 - 绝对坐标，1 - 相对坐标，2 - 局部坐标
        std::uint8_t type;
        tokenReader.push();
        ASTNode relativeNotation = nodeRelativeNotation->getASTNodeWithNextNode(tokenReader, cpack);
        tokenReader.restore();
        if (!relativeNotation.isError()) {
            //相对坐标
            type = 1;
        } else {
            tokenReader.push();
            ASTNode caretNotation = nodeCaretNotation->getASTNodeWithNextNode(tokenReader, cpack);
            tokenReader.restore();
            if (!caretNotation.isError()) {
                //局部坐标
                type = 2;
            } else {
                //绝对坐标
                type = 0;
            }
        }
        //数值部分
        ASTNode result = nodeRelativeFloat->getASTNodeWithNextNode(tokenReader, cpack);
        //判断有没有错误
        if (type == 2 && !canUseCaretNotation) {
            result = ASTNode::andNode(node, {result}, result.tokens, ErrorReason::logicError(
                    result.tokens, "不能使用局部坐标"), "relativeFloat");
        } else if (result.isError()) {
            result = ASTNode::andNode(node, {result}, result.tokens,
                                      ErrorReason::typeError(result.tokens, FormatUtil::format(
                                              "类型不匹配，{0}不是有效的坐标参数", TokenUtil::toString(result.tokens))),
                                      "relativeFloat");
        }
        return {type, result};
    }

    void NodeRelativeFloat::collectStructure(const ASTNode *astNode,
                                             StructureBuilder &structure,
                                             bool isMustHave) const {
        structure.append(isMustHave, description.value_or("坐标"));
    }

} // CHelper::Node