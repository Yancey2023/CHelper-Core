//
// Created by Yancey on 2024/2/28.
//

#include <chelper/node/json/NodeJsonNull.h>

namespace CHelper::Node {

    NodeJsonNull::NodeJsonNull(const std::optional<std::u16string> &id,
                               const std::optional<std::u16string> &description)
        : NodeBase(id, description, false) {}

    NodeTypeId::NodeTypeId NodeJsonNull::getNodeType() const {
        return NodeTypeId::JSON_NULL;
    }

    ASTNode NodeJsonNull::getASTNode(TokenReader &tokenReader, const CPack *cpack) const {
        tokenReader.push();
        auto result = tokenReader.readStringASTNode(this);
        tokenReader.pop();
        std::u16string_view str = result.tokens.toString();
        if (HEDLEY_LIKELY(str.empty())) {
            TokensView tokens = result.tokens;
            return ASTNode::andNode(this, {std::move(result)}, tokens, ErrorReason::contentError(tokens, u"null参数为空"));
        } else if (HEDLEY_LIKELY(str != u"null")) {
            TokensView tokens = result.tokens;
            return ASTNode::andNode(this, {std::move(result)}, tokens, ErrorReason::contentError(tokens, fmt::format(u"内容不是null -> {}", str)));
        }
        return result;
    }

    bool NodeJsonNull::collectSuggestions(const ASTNode *astNode,
                                          size_t index,
                                          std::vector<Suggestions> &suggestions) const {
        std::u16string_view str = astNode->tokens.toString().substr(0, index - astNode->tokens.getStartIndex());
        if (HEDLEY_LIKELY(str.find(u"null") != std::u16string::npos)) {
            std::shared_ptr<NormalId> id;
            id->name = u"null";
            id->description = u"null参数";
            suggestions.push_back(Suggestions::singleLiteralSuggestion({astNode->tokens, false, id}));
        }
        return true;
    }

    bool NodeJsonNull::collectSyntax(const ASTNode *astNode,
                                     SyntaxResult &syntaxResult) const {
        syntaxResult.update(astNode->tokens, SyntaxTokenType::NULL_TOKEN);
        return true;
    }

}// namespace CHelper::Node