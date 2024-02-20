//
// Created by Yancey666 on 2023/12/2.
//

#include "NodePosition.h"
#include "util/NodeSingleSymbol.h"
#include "NodeFloat.h"
#include "../../../util/StringUtil.h"

namespace CHelper::Node {

    //TODO 检测逻辑与NodeRelativeFloat相同，是否可以合并代码
    static NodeSingleSymbol nodeRelativeNotation("RELATIVE_FLOAT_CARET_NOTATION", "相对坐标（~x ~y ~z）", '~');
    static NodeSingleSymbol nodeCaretNotation("RELATIVE_FLOAT_CARET_NOTATION", "局部坐标（^左 ^上 ^右）", '^');
    static NodeFloat nodeFloat("RELATIVE_FLOAT_FLOAT", "相对坐标的数值", std::nullopt, std::nullopt);

    NODE_TYPE("POSITION", NodePosition)

    NodePosition::NodePosition(const std::optional<std::string> &id,
                               const std::optional<std::string> &description)
            : NodeBase(id, description) {}

    NodePosition::NodePosition(const nlohmann::json &j,
                               const CPack &cpack)
            : NodeBase(j, cpack) {}

    ASTNode NodePosition::getASTNode(TokenReader &tokenReader, const CPack &cpack) const {
        tokenReader.push();
        // 0 - 绝对坐标，1 - 相对坐标，2 - 局部坐标
        std::vector<ASTNode> threeChildNodes;
        int types[3];
        for (int &type: types) {
            tokenReader.push();
            tokenReader.push();
            std::vector<ASTNode> childNodes;
            ASTNode relativeNotation = nodeRelativeNotation.getASTNode(tokenReader, cpack);
            if (!relativeNotation.isError()) {
                //相对坐标
                tokenReader.pop();
                childNodes.push_back(relativeNotation);
                type = 1;
            } else {
                tokenReader.restore();
                tokenReader.push();
                ASTNode caretNotation = nodeCaretNotation.getASTNode(tokenReader, cpack);
                if (!caretNotation.isError()) {
                    //局部坐标
                    tokenReader.pop();
                    childNodes.push_back(caretNotation);
                    type = 2;
                } else {
                    //绝对坐标
                    tokenReader.restore();
                    type = 0;
                }
            }
            //数值部分
            childNodes.push_back(nodeFloat.getASTNode(tokenReader, cpack));
            //判断有没有错误
            VectorView<Token> tokens = tokenReader.collect();
            ASTNode node = ASTNode::andNode(this, childNodes, tokens, ErrorReason::errorType(tokens, util::Format(
                    "类型不匹配，{0}不是有效的坐标参数", StringUtil::toString(tokens))));
            if (node.isError()) {
                threeChildNodes.push_back(ASTNode::andNode(this, childNodes, tokens,
                                                           ErrorReason::errorType(tokens, util::Format(
                                                                   "类型不匹配，{0}不是有效的坐标参数",
                                                                   StringUtil::toString(tokens)))));
            } else {
                threeChildNodes.push_back(ASTNode::andNode(this, childNodes, tokens));
            }
        }
        //判断有没有错误
        VectorView<Token> tokens = tokenReader.collect();
        ASTNode result = ASTNode::andNode(this, threeChildNodes, tokens);
        if (!result.isError()) {
            int type = 0;
            for (int i: types) {
                if (i == 0 || i == type) {
                    continue;
                } else if (type == 0) {
                    type = i;
                } else {
                    return ASTNode::andNode(this, threeChildNodes, tokenReader.collect(),
                                            ErrorReason::other(tokens, "坐标参数不能同时使用相对坐标和局部坐标"));
                }
            }
        }
        return result;
    }

} // CHelper::Node