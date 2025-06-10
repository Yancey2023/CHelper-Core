//
// Created by Yancey on 2023/12/19.
//

#include <chelper/node/util/NodeSingleSymbol.h>

namespace CHelper::Node {

    static std::shared_ptr<NormalId> getNormalId(char16_t symbol, const std::optional<std::u16string> &description) {
        std::u16string name;
        name.push_back(symbol);
        return NormalId::make(name, description);
    }

    NodeSingleSymbol::NodeSingleSymbol(char16_t symbol,
                                       const std::optional<std::u16string> &description,
                                       bool isAddSpace)
        : symbol(symbol),
          normalId(getNormalId(symbol, description)),
          isAddSpace(isAddSpace) {}

    NodeTypeId::NodeTypeId NodeSingleSymbol::getNodeType() const {
        return NodeTypeId::SINGLE_SYMBOL;
    }

    ASTNode NodeSingleSymbol::getASTNode(TokenReader &tokenReader, const CPack *cpack) const {
        ASTNode symbolNode = tokenReader.readSymbolASTNode(this);
        std::shared_ptr<ErrorReason> errorReason;
        if (HEDLEY_UNLIKELY(symbolNode.isError())) {
            if (HEDLEY_LIKELY(symbolNode.tokens.isEmpty())) {
                return ASTNode::simpleNode(this, symbolNode.tokens, ErrorReason::incomplete(symbolNode.tokens, fmt::format(u"命令不完整，需要符号{:c}", symbol)));
            } else {
                return ASTNode::simpleNode(this, symbolNode.tokens, ErrorReason::typeError(symbolNode.tokens, fmt::format(u"类型不匹配，需要符号{:c}，但当前内容为{}", symbol, symbolNode.tokens.toString())));
            }
        }
        std::u16string_view str = symbolNode.tokens.toString();
        if (HEDLEY_LIKELY(str.length() == 1 && str[0] == symbol)) {
            return symbolNode;
        }
        return ASTNode::simpleNode(this, symbolNode.tokens, ErrorReason::contentError(symbolNode.tokens, fmt::format(u"内容不匹配，正确的符号为{:c}，但当前内容为{}", symbol, str)));
    }

    bool NodeSingleSymbol::collectSuggestions(const ASTNode *astNode,
                                              size_t index,
                                              std::vector<Suggestions> &suggestions) const {
        if (HEDLEY_LIKELY(astNode->tokens.getStartIndex() != index)) {
            return true;
        }
        suggestions.push_back(Suggestions::singleSymbolSuggestion({index, index, isAddSpace, normalId}));
        return true;
    }

    bool NodeSingleSymbol::collectSyntax(const ASTNode *astNode,
                                         SyntaxResult &syntaxResult) const {
        syntaxResult.update(astNode->tokens, SyntaxTokenType::SYMBOL);
        return true;
    }

}// namespace CHelper::Node