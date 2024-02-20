//
// Created by Yancey666 on 2023/12/20.
//

#include "NodeRelativeFloat.h"
#include "../../../util/StringUtil.h"
#include "util/NodeSingleSymbol.h"
#include "NodeFloat.h"


namespace CHelper::Node {

    static NodeSingleSymbol nodeRelativeNotation("RELATIVE_FLOAT_CARET_NOTATION", "相对坐标（~x ~y ~z）", '~');
    static NodeSingleSymbol nodeCaretNotation("RELATIVE_FLOAT_CARET_NOTATION", "局部坐标（^左 ^上 ^右）", '^');
    static NodeFloat nodeFloat("RELATIVE_FLOAT_FLOAT", "相对坐标的数值", std::nullopt, std::nullopt);

    NODE_TYPE("RELATIVE_FLOAT", NodeRelativeFloat)

    NodeRelativeFloat::NodeRelativeFloat(const std::optional<std::string> &id,
                                         const std::optional<std::string> &description,
                                         bool canUseCaretNotation)
            : NodeBase(id, description),
              canUseCaretNotation(canUseCaretNotation) {}

    NodeRelativeFloat::NodeRelativeFloat(const nlohmann::json &j,
                                         const CPack &cpack)
            : NodeBase(j, cpack),
              canUseCaretNotation(FROM_JSON(j, canUseCaretNotation, bool)) {}

    void NodeRelativeFloat::toJson(nlohmann::json &j) const {
        NodeBase::toJson(j);
        TO_JSON(j, canUseCaretNotation);
    }

    ASTNode NodeRelativeFloat::getASTNode(TokenReader &tokenReader, const CPack &cpack) const {
        tokenReader.push();
        tokenReader.push();
        std::vector<ASTNode> childNodes;
        ASTNode relativeNotation = nodeRelativeNotation.getASTNode(tokenReader, cpack);
        bool isUseCaretNotation = false;
        if (!relativeNotation.isError()) {
            //相对坐标
            tokenReader.pop();
            childNodes.push_back(relativeNotation);
        } else {
            tokenReader.restore();
            tokenReader.push();
            ASTNode caretNotation = nodeCaretNotation.getASTNode(tokenReader, cpack);
            if (!caretNotation.isError()) {
                //局部坐标
                isUseCaretNotation = true;
                tokenReader.pop();
                childNodes.push_back(caretNotation);
            } else {
                //绝对坐标
                tokenReader.restore();
            }
        }
        //数值部分
        childNodes.push_back(nodeFloat.getASTNode(tokenReader, cpack));
        //判断有没有错误
        VectorView<Token> tokens = tokenReader.collect();
        if (isUseCaretNotation && !canUseCaretNotation) {
            return ASTNode::andNode(this, childNodes, tokens, ErrorReason::other(tokens, "不能使用局部坐标"));
        }
        ASTNode result = ASTNode::andNode(this, childNodes, tokens);
        if (result.isError()) {
            return ASTNode::andNode(this, childNodes, tokens, ErrorReason::errorType(tokens, util::Format(
                    "类型不匹配，{0}不是有效的坐标参数", StringUtil::toString(tokens))));
        } else {
            return result;
        }
    }

} // CHelper::Node