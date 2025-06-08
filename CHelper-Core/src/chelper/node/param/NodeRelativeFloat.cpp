//
// Created by Yancey on 2023/12/20.
//

#include <chelper/node/param/NodeRelativeFloat.h>
#include <chelper/node/util/NodeOr.h>
#include <chelper/node/util/NodeSingleSymbol.h>

namespace CHelper::Node {

    static std::unique_ptr<NodeSingleSymbol> nodeRelativeNotation = std::make_unique<NodeSingleSymbol>(
            u"RELATIVE_FLOAT_RELATIVE_NOTATION", u"相对坐标（~x ~y ~z）", u'~', false);
    static std::unique_ptr<NodeSingleSymbol> nodeCaretNotation = std::make_unique<NodeSingleSymbol>(
            u"RELATIVE_FLOAT_CARET_NOTATION", u"局部坐标（^左 ^上 ^前）", u'^', false);
    static std::unique_ptr<NodeOr> nodePreSymbol = std::make_unique<NodeOr>(
            u"RELATIVE_FLOAT_RELATIVE", u"相对坐标（~x ~y ~z）",
            std::vector<const NodeBase *>{nodeRelativeNotation.get(), nodeCaretNotation.get()}, false);

    NodeRelativeFloat::NodeRelativeFloat(const std::optional<std::u16string> &id,
                                         const std::optional<std::u16string> &description,
                                         bool canUseCaretNotation)
        : NodeBase(id, description, false),
          canUseCaretNotation(canUseCaretNotation) {}

    NodeTypeId::NodeTypeId NodeRelativeFloat::getNodeType() const {
        return NodeTypeId::RELATIVE_FLOAT;
    }

    ASTNode NodeRelativeFloat::getASTNode(TokenReader &tokenReader, const CPack *cpack) const {
        std::pair<NodeRelativeFloatType::NodeRelativeFloatType, ASTNode> result = getASTNode(this, cpack, tokenReader);
        if (HEDLEY_UNLIKELY(result.second.isError())) {
            return std::move(result.second);
        }
        if (HEDLEY_UNLIKELY(!canUseCaretNotation && result.first == 2)) {
            TokensView tokens = result.second.tokens;
            return ASTNode::andNode(this, {std::move(result.second)}, tokens, nullptr, ASTNodeId::NODE_RELATIVE_FLOAT_WITH_ERROR);
        }
        return result.second;
    }

    bool NodeRelativeFloat::collectIdError(const ASTNode *astNode,
                                           std::vector<std::shared_ptr<ErrorReason>> &idErrorReasons) const {
        if (HEDLEY_UNLIKELY(!astNode->isError() && astNode->id == ASTNodeId::NODE_RELATIVE_FLOAT_WITH_ERROR)) {
            idErrorReasons.push_back(ErrorReason::logicError(astNode->tokens, u"不能使用局部坐标"));
            return true;
        } else {
            return false;
        }
    }

    std::pair<NodeRelativeFloatType::NodeRelativeFloatType, ASTNode>
    NodeRelativeFloat::getASTNode(const NodeBase *node,
                                  const CPack *cpack,
                                  TokenReader &tokenReader) {
        tokenReader.push();
        std::vector<ASTNode> childNodes;
        // 0 - 绝对坐标，1 - 相对坐标，2 - 局部坐标
        NodeRelativeFloatType::NodeRelativeFloatType type;
        tokenReader.push();
        ASTNode preSymbol = nodePreSymbol->getASTNode(tokenReader, cpack);
        if (HEDLEY_UNLIKELY(preSymbol.isError())) {
            type = NodeRelativeFloatType::ABSOLUTE_COORDINATE;
            tokenReader.restore();
        } else {
            if (HEDLEY_LIKELY(preSymbol.childNodes[1].isError())) {
                type = NodeRelativeFloatType::RELATIVE_WORLD_COORDINATE;
            } else {
                type = NodeRelativeFloatType::LOCAL_COORDINATE;
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
        ASTNode number = tokenReader.readFloatASTNode(node, ASTNodeId::NODE_RELATIVE_FLOAT_NUMBER);
        std::shared_ptr<ErrorReason> errorReason;
        if (HEDLEY_LIKELY(!number.isError())) {
            tokenReader.pop();
        } else if (HEDLEY_UNLIKELY(childNodes.empty())) {
            tokenReader.pop();
            TokensView tokens = number.tokens;
            errorReason = ErrorReason::typeError(tokens, fmt::format(u"类型不匹配，{}不是有效的坐标参数", tokens.toString()));
        } else {
            tokenReader.restore();
        }
        childNodes.push_back(std::move(number));
        ASTNode result = ASTNode::andNode(node, std::move(childNodes), tokenReader.collect(), errorReason);
        // 为了获取补全提示，再嵌套一层or节点
        return {type, ASTNode::orNode(node, {std::move(result), std::move(preSymbol)}, nullptr)};
    }

    bool NodeRelativeFloat::collectSuggestions(const ASTNode *astNode, size_t index, std::vector<Suggestions> &suggestions) const {
        std::u16string_view str = astNode->tokens.toString();
        size_t startIndex = astNode->tokens.getStartIndex();
        for (size_t i = 0; i < str.length(); ++i) {
            if (HEDLEY_UNLIKELY(startIndex + i == index)) {
                return collectSuggestions(index, suggestions, canUseCaretNotation);
            }
            if (str[i] != ' ') {
                return true;
            }
        }
        if (HEDLEY_UNLIKELY(startIndex + str.length() == index)) {
            return collectSuggestions(index, suggestions, canUseCaretNotation);
        }
        return true;
    }

    bool NodeRelativeFloat::collectSuggestions(size_t index, std::vector<Suggestions> &suggestions, bool canUseCaretNotation) {
        suggestions.push_back(Suggestions::singleWhitespaceSuggestion({index, index, false, whitespaceId}));
        suggestions.push_back(Suggestions::singleSymbolSuggestion({index, index, false, nodeRelativeNotation->normalId}));
        if (HEDLEY_LIKELY(canUseCaretNotation)) {
            suggestions.push_back(Suggestions::singleSymbolSuggestion({index, index, false, nodeCaretNotation->normalId}));
        }
        return true;
    }

    void NodeRelativeFloat::collectStructure(const ASTNode *astNode,
                                             StructureBuilder &structure,
                                             bool isMustHave) const {
        structure.append(isMustHave, description.value_or(u"坐标"));
    }

    bool NodeRelativeFloat::collectSyntax(const ASTNode *astNode,
                                          SyntaxResult &syntaxResult) const {
        if (astNode->id == ASTNodeId::NODE_RELATIVE_FLOAT_NUMBER) {
            syntaxResult.update(astNode->tokens, SyntaxTokenType::FLOAT);
            return true;
        } else {
            return false;
        }
    }

}// namespace CHelper::Node