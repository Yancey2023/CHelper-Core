//
// Created by Yancey on 2023/12/20.
//

#include "NodeRelativeFloat.h"
#include "../util/NodeOr.h"
#include "../util/NodeSingleSymbol.h"

namespace CHelper::Node {

    static std::unique_ptr<NodeSingleSymbol> nodeRelativeNotation = std::make_unique<NodeSingleSymbol>(
            "RELATIVE_FLOAT_RELATIVE_NOTATION", "相对坐标（~x ~y ~z）", '~', false);
    static std::unique_ptr<NodeSingleSymbol> nodeCaretNotation = std::make_unique<NodeSingleSymbol>(
            "RELATIVE_FLOAT_CARET_NOTATION", "局部坐标（^左 ^上 ^前）", '^', false);
    static std::unique_ptr<NodeOr> nodePreSymbol = std::make_unique<NodeOr>(
            "RELATIVE_FLOAT_RELATIVE", "相对坐标（~x ~y ~z）",
            std::vector<const NodeBase *>{nodeRelativeNotation.get(), nodeCaretNotation.get()}, false);

    NodeRelativeFloat::NodeRelativeFloat(const std::optional<std::string> &id,
                                         const std::optional<std::string> &description,
                                         bool canUseCaretNotation)
        : NodeBase(id, description, false),
          canUseCaretNotation(canUseCaretNotation) {}

    NodeType *NodeRelativeFloat::getNodeType() const {
        return NodeType::RELATIVE_FLOAT.get();
    }

    ASTNode NodeRelativeFloat::getASTNode(TokenReader &tokenReader, const CPack *cpack) const {
        std::pair<uint8_t, ASTNode> result = getASTNode(this, cpack, tokenReader);
        if (HEDLEY_UNLIKELY(result.second.isError())) {
            return std::move(result.second);
        }
        if (HEDLEY_UNLIKELY(!canUseCaretNotation && result.first == 2)) {
            TokensView tokens = result.second.tokens;
            return ASTNode::andNode(this, {std::move(result.second)}, tokens, ErrorReason::logicError(tokens, "不能使用局部坐标"));
        }
        return result.second;
    }

    std::pair<uint8_t, ASTNode> NodeRelativeFloat::getASTNode(const NodeBase *node,
                                                              const CPack *cpack,
                                                              TokenReader &tokenReader) {
        tokenReader.push();
        std::vector<ASTNode> childNodes;
        // 0 - 绝对坐标，1 - 相对坐标，2 - 局部坐标
        uint8_t type;
        tokenReader.push();
        ASTNode preSymbol = nodePreSymbol->getASTNode(tokenReader, cpack);
        if (HEDLEY_UNLIKELY(preSymbol.isError())) {
            type = 0;
            tokenReader.restore();
        } else {
            if (HEDLEY_LIKELY(preSymbol.childNodes[1].isError())) {
                type = 1;
            } else {
                type = 2;
            }
            tokenReader.pop();
            //空格检测
            if (HEDLEY_UNLIKELY(tokenReader.ready() && tokenReader.peek()->type == TokenType::WHITE_SPACE)) {
                childNodes.push_back(std::move(preSymbol));
                return {type, ASTNode::andNode(node, std::move(childNodes), tokenReader.collect())};
            }
            childNodes.push_back(preSymbol);
        }
        //数值部分
        tokenReader.push();
        ASTNode number = tokenReader.readFloatASTNode(node);
        std::shared_ptr<ErrorReason> errorReason;
        if (HEDLEY_LIKELY(!number.isError())) {
            tokenReader.pop();
        } else if (HEDLEY_UNLIKELY(childNodes.empty())) {
            tokenReader.pop();
            TokensView tokens = number.tokens;
            errorReason = ErrorReason::typeError(tokens, fmt::format("类型不匹配，{0}不是有效的坐标参数", tokens.toString()));
        } else {
            tokenReader.restore();
        }
        childNodes.push_back(std::move(number));
        ASTNode result = ASTNode::andNode(node, std::move(childNodes), tokenReader.collect(), errorReason);
        // 为了获取补全提示，再嵌套一层or节点
        return {type, ASTNode::orNode(node, {std::move(result), std::move(preSymbol)}, nullptr)};
    }

    bool NodeRelativeFloat::collectSuggestions(const ASTNode *astNode, size_t index, std::vector<Suggestions> &suggestions) const {
        std::string_view str = astNode->tokens.toString();
        size_t startIndex = astNode->tokens.getStartIndex();
        for (int i = 0; i < str.length(); ++i) {
            if (HEDLEY_UNLIKELY(startIndex + i == index)) {
                return collectSuggestions(index, suggestions, canUseCaretNotation);
            }
            if (str[i] != ' ') {
                return false;
            }
        }
        if (HEDLEY_UNLIKELY(startIndex + str.length() == index)) {
            return collectSuggestions(index, suggestions, canUseCaretNotation);
        }
        return true;
    }

    bool NodeRelativeFloat::collectSuggestions(size_t index, std::vector<Suggestions> &suggestions, bool canUseCaretNotation) {
        suggestions.push_back(Suggestions::singleSuggestion({index, index, false, whitespaceId}));
        suggestions.push_back(Suggestions::singleSuggestion({index, index, false, nodeRelativeNotation->normalId}));
        if (HEDLEY_LIKELY(canUseCaretNotation)) {
            suggestions.push_back(Suggestions::singleSuggestion({index, index, false, nodeCaretNotation->normalId}));
        }
        return true;
    }

    void NodeRelativeFloat::collectStructure(const ASTNode *astNode,
                                             StructureBuilder &structure,
                                             bool isMustHave) const {
        structure.append(isMustHave, description.value_or("坐标"));
    }

    CODEC_NODE(NodeRelativeFloat, canUseCaretNotation)

}// namespace CHelper::Node