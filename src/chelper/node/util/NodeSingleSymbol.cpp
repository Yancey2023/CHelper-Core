//
// Created by Yancey on 2023/12/19.
//

#include "NodeSingleSymbol.h"
#include "../../util/TokenUtil.h"

namespace CHelper::Node {

    NodeSingleSymbol::NodeSingleSymbol(const std::optional<std::string> &id,
                                       const std::optional<std::string> &description,
                                       char symbol)
            : NodeBase(id, description, false),
              symbol(symbol) {}

    ASTNode NodeSingleSymbol::getASTNode(TokenReader &tokenReader, const CPack *cpack) const {
        ASTNode symbolNode = tokenReader.readSymbolASTNode(this);
        std::shared_ptr<ErrorReason> errorReason;
        if (symbolNode.isError()) {
            if (symbolNode.tokens.isEmpty()) {
                return ASTNode::simpleNode(this, symbolNode.tokens, ErrorReason::incomplete(
                        symbolNode.tokens, FormatUtil::format(
                                "指令不完整，需要符号{0}", symbol)));
            } else {
                return ASTNode::simpleNode(this, symbolNode.tokens, ErrorReason::typeError(
                        symbolNode.tokens, FormatUtil::format(
                                "类型不匹配，需要符号{0}，但当前内容为{1}",
                                symbol, TokenUtil::toString(symbolNode.tokens))));
            }
        }
        std::string str = TokenUtil::toString(symbolNode.tokens);
        if (str.length() == 1 && str[0] == symbol) {
            return symbolNode;
        }
        return ASTNode::simpleNode(this, symbolNode.tokens, ErrorReason::contentError(
                symbolNode.tokens, FormatUtil::format(
                        "内容不匹配，正确的符号为{0}，但当前内容为{1}", symbol, str)));
    }

    bool NodeSingleSymbol::collectSuggestions(const ASTNode *astNode,
                                              size_t index,
                                              std::vector<Suggestion> &suggestions) const {
        if (TokenUtil::getStartIndex(astNode->tokens) != index) {
            return true;
        }
        std::string name;
        name.push_back(symbol);
        suggestions.emplace_back(astNode->tokens, std::make_shared<NormalId>(name, description));
        return true;
    }

} // CHelper::Node