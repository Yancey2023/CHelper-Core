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

    ASTNode NodeSingleSymbol::getASTNode(TokenReader &tokenReader) const {
        ASTNode symbolNode = tokenReader.getSymbolASTNode(this, "single node");
        std::shared_ptr<ErrorReason> errorReason;
        if (symbolNode.isError()) {
            return symbolNode;
        }
        std::string str = TokenUtil::toString(symbolNode.tokens);
        if (str.length() == 1 && str[0] == symbol) {
            return symbolNode;
        }
        return ASTNode::simpleNode(this, symbolNode.tokens, ErrorReason::contentError(
                symbolNode.tokens, FormatUtil::format(
                        "内容不匹配，正确的符号为{0}，但当前内容为{1}",
                        symbol, str)), "single node");
    }

    bool NodeSingleSymbol::collectSuggestions(const ASTNode *astNode, size_t index,
                                              std::vector<Suggestion> &suggestions) const {
        std::string name;
        name.push_back(symbol);
        suggestions.emplace_back(astNode->tokens, std::make_shared<NormalId>(name, description));
        return true;
    }

} // CHelper::Node