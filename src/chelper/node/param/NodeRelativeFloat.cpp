//
// Created by Yancey666 on 2023/12/20.
//

#include "NodeRelativeFloat.h"
#include "../../util/TokenUtil.h"
#include "../util/NodeSingleSymbol.h"
#include "NodeFloat.h"

namespace CHelper::Node {

    static NodeSingleSymbol nodeRelativeNotation("RELATIVE_FLOAT_CARET_NOTATION", "相对坐标（~x ~y ~z）", '~');
    static NodeSingleSymbol nodeCaretNotation("RELATIVE_FLOAT_CARET_NOTATION", "局部坐标（^左 ^上 ^右）", '^');
    static NodeFloat nodeValue("RELATIVE_FLOAT_FLOAT", "坐标参数的数值", std::nullopt, std::nullopt);

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
        return getASTNode(this, tokenReader, cpack, canUseCaretNotation).second;
    }

    std::pair<int, ASTNode> NodeRelativeFloat::getASTNode(const NodeBase *node,
                                                          TokenReader &tokenReader,
                                                          const CPack &cpack,
                                                          bool canUseCaretNotation) {
        tokenReader.push();
        tokenReader.push();
        // 0 - 绝对坐标，1 - 相对坐标，2 - 局部坐标
        int type;
        std::vector<ASTNode> childNodes;
        ASTNode relativeNotation = nodeRelativeNotation.getASTNode(tokenReader, cpack);
        bool isUseCaretNotation = false;
        if (!relativeNotation.isError()) {
            //相对坐标
            type = 1;
            tokenReader.pop();
            childNodes.push_back(relativeNotation);
        } else {
            tokenReader.restore();
            tokenReader.push();
            ASTNode caretNotation = nodeCaretNotation.getASTNode(tokenReader, cpack);
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
        childNodes.push_back(nodeValue.getASTNode(tokenReader, cpack));
        //判断有没有错误
        VectorView<Token> tokens = tokenReader.collect();
        if (isUseCaretNotation && !canUseCaretNotation) {
            return {type, ASTNode::andNode(node, childNodes, tokens, ErrorReason::other(tokens, "不能使用局部坐标"))};
        }
        ASTNode result = ASTNode::andNode(node, childNodes, tokens);
        if (result.isError()) {
            return {type, ASTNode::andNode(node, childNodes, tokens, ErrorReason::errorType(tokens, FormatUtil::format(
                    "类型不匹配，{0}不是有效的坐标参数", TokenUtil::toString(tokens))))};
        } else {
            return {type, result};
        }
    }

} // CHelper::Node