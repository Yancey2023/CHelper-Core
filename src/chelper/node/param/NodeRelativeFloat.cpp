//
// Created by Yancey on 2023/12/20.
//

#include "NodeRelativeFloat.h"
#include "../../util/TokenUtil.h"
#include "../util/NodeSingleSymbol.h"
#include "NodeFloat.h"

namespace CHelper::Node {

    static NodeSingleSymbol nodeRelativeNotation("RELATIVE_FLOAT_CARET_NOTATION", "相对坐标（~x ~y ~z）", '~');
    static NodeSingleSymbol nodeCaretNotation("RELATIVE_FLOAT_CARET_NOTATION", "局部坐标（^左 ^上 ^右）", '^');
    static NodeFloat nodeValue("RELATIVE_FLOAT_FLOAT", "坐标参数的数值", std::nullopt, std::nullopt);

    NodeRelativeFloat::NodeRelativeFloat(const std::optional<std::string> &id,
                                         const std::optional<std::string> &description,
                                         bool canUseCaretNotation)
            : NodeBase(id, description, false),
              canUseCaretNotation(canUseCaretNotation) {}

    NodeRelativeFloat::NodeRelativeFloat(const nlohmann::json &j,
                                         const CPack &cpack)
            : NodeBase(j),
              canUseCaretNotation(FROM_JSON(j, canUseCaretNotation, bool)) {}

    NodeType NodeRelativeFloat::getNodeType() const {
        return NodeType::RELATIVE_FLOAT;
    }

    void NodeRelativeFloat::toJson(nlohmann::json &j) const {
        NodeBase::toJson(j);
        TO_JSON(j, canUseCaretNotation);
    }

    ASTNode NodeRelativeFloat::getASTNode(TokenReader &tokenReader) const {
        return getASTNode(this, tokenReader, canUseCaretNotation).second;
    }

    std::pair<int, ASTNode> NodeRelativeFloat::getASTNode(const NodeBase *node,
                                                          TokenReader &tokenReader,
                                                          bool canUseCaretNotation) {
        tokenReader.push();
        tokenReader.push();
        // 0 - 绝对坐标，1 - 相对坐标，2 - 局部坐标
        int type;
        std::vector<ASTNode> childNodes;
        ASTNode relativeNotation = nodeRelativeNotation.getASTNode(tokenReader);
        bool isUseCaretNotation = false;
        if (!relativeNotation.isError()) {
            //相对坐标
            type = 1;
            tokenReader.pop();
            childNodes.push_back(relativeNotation);
        } else {
            tokenReader.restore();
            tokenReader.push();
            ASTNode caretNotation = nodeCaretNotation.getASTNode(tokenReader);
            if (!caretNotation.isError()) {
                //局部坐标
                type = 2;
                isUseCaretNotation = true;
                tokenReader.pop();
                childNodes.push_back(caretNotation);
            } else {
                //绝对坐标
                type = 0;
                tokenReader.restore();
            }
        }
        //数值部分
        childNodes.push_back(nodeValue.getASTNode(tokenReader));
        //判断有没有错误
        VectorView <Token> tokens = tokenReader.collect();
        if (isUseCaretNotation && !canUseCaretNotation) {
            return {type, ASTNode::andNode(node, childNodes, tokens,
                                           ErrorReason::logicError(tokens, "不能使用局部坐标"),
                                           "relativeFloat")};
        }
        ASTNode result = ASTNode::andNode(node, childNodes, tokens, nullptr, "relativeFloat");
        if (result.isError()) {
            return {type, ASTNode::andNode(node, childNodes, tokens, ErrorReason::typeError(tokens, FormatUtil::format(
                    "类型不匹配，{0}不是有效的坐标参数", TokenUtil::toString(tokens))), "relativeFloat")};
        } else {
            return {type, result};
        }
    }

    void NodeRelativeFloat::collectStructure(const ASTNode *astNode,
                                             StructureBuilder &structure,
                                             bool isMustHave) const {
        if (astNode == nullptr || astNode->id == "relativeFloat") {
            NodeBase::collectStructure(astNode, structure, isMustHave);
        }
    }

} // CHelper::Node