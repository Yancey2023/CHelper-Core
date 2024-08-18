//
// Created by Yancey on 2024/2/28.
//

#include "NodeJsonBoolean.h"

namespace CHelper::Node {

    NodeJsonBoolean::NodeJsonBoolean(const std::optional<std::wstring> &id,
                                     const std::optional<std::wstring> &description,
                                     const std::optional<std::wstring> &descriptionTrue,
                                     const std::optional<std::wstring> &descriptionFalse)
        : NodeBase(id, description, false),
          descriptionTrue(descriptionTrue),
          descriptionFalse(descriptionFalse) {}

    NodeType *NodeJsonBoolean::getNodeType() const {
        return NodeType::JSON_BOOLEAN.get();
    }

    ASTNode NodeJsonBoolean::getASTNode(TokenReader &tokenReader, const CPack *cpack) const {
        ASTNode astNode = tokenReader.readStringASTNode(this);
        std::wstring_view str = astNode.tokens.toString();
        if (HEDLEY_UNLIKELY(str == L"true" || str == L"false")) {
            return astNode;
        }
        TokensView tokens = astNode.tokens;
        return ASTNode::andNode(this, {std::move(astNode)}, tokens, ErrorReason::contentError(tokens, L"内容不匹配，应该为布尔值，但当前内容为" + std::wstring(str)));
    }

    bool NodeJsonBoolean::collectSuggestions(const ASTNode *astNode,
                                             size_t index,
                                             std::vector<Suggestions> &suggestions) const {
        KMPMatcher kmpMatcher(astNode->tokens.toString().substr(0, index - astNode->tokens.getStartIndex()));
        Suggestions suggestions1{SuggestionsType::LITERAL};
        if (HEDLEY_UNLIKELY(kmpMatcher.match(L"true") != std::wstring::npos)) {
            std::shared_ptr<NormalId> id;
            id->name = L"true";
            id->description = descriptionTrue;
            suggestions1.suggestions.emplace_back(astNode->tokens, false, std::move(id));
        }
        if (HEDLEY_UNLIKELY(kmpMatcher.match(L"false") != std::wstring::npos)) {
            std::shared_ptr<NormalId> id;
            id->name = L"false";
            id->description = descriptionFalse;
            suggestions1.suggestions.emplace_back(astNode->tokens, false, std::move(id));
        }
        suggestions1.markFiltered();
        suggestions.push_back(std::move(suggestions1));
        return true;
    }

    bool NodeJsonBoolean::collectColor(const ASTNode *astNode,
                                       ColoredString &coloredString,
                                       const Theme &theme) const {
        coloredString.setColor(astNode->tokens, theme.colorBoolean);
        return true;
    }

    CODEC_NODE(NodeJsonBoolean, descriptionTrue, descriptionFalse)

}// namespace CHelper::Node