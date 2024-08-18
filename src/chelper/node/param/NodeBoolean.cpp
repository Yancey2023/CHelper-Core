//
// Created by Yancey on 2023/11/11.
//

#include "NodeBoolean.h"

namespace CHelper::Node {

    NodeBoolean::NodeBoolean(const std::optional<std::wstring> &id,
                             const std::optional<std::wstring> &description,
                             const std::optional<std::wstring> &descriptionTrue,
                             const std::optional<std::wstring> &descriptionFalse)
        : NodeBase(id, description, false),
          descriptionTrue(descriptionTrue),
          descriptionFalse(descriptionFalse) {}

    NodeType *NodeBoolean::getNodeType() const {
        return NodeType::BOOLEAN.get();
    }

    ASTNode NodeBoolean::getASTNode(TokenReader &tokenReader, const CPack *cpack) const {
        ASTNode astNode = tokenReader.readStringASTNode(this);
        std::wstring_view str = astNode.tokens.toString();
        if (HEDLEY_LIKELY(str == L"true" || str == L"false")) {
            return astNode;
        }
        TokensView tokens = astNode.tokens;
        return ASTNode::andNode(this, {std::move(astNode)}, tokens, ErrorReason::contentError(tokens, L"内容不匹配，应该为布尔值，但当前内容为" + std::wstring(str)));
    }

    bool NodeBoolean::collectSuggestions(const ASTNode *astNode,
                                         size_t index,
                                         std::vector<Suggestions> &suggestions) const {
        KMPMatcher kmpMatcher(astNode->tokens.toString().substr(0, index - astNode->tokens.getStartIndex()));
        Suggestions suggestions1(SuggestionsType::LITERAL);
        if (HEDLEY_UNLIKELY(kmpMatcher.match(L"true") != std::wstring::npos)) {
            suggestions1.suggestions.emplace_back(
                    astNode->tokens, true,
                    NormalId::make(L"true", descriptionTrue));
        }
        if (HEDLEY_UNLIKELY(kmpMatcher.match(L"false") != std::wstring::npos)) {
            suggestions1.suggestions.emplace_back(
                    astNode->tokens, true,
                    NormalId::make(L"false", descriptionFalse));
        }
        suggestions1.markFiltered();
        suggestions.push_back(std::move(suggestions1));
        return true;
    }

    void NodeBoolean::collectStructure(const ASTNode *astNode,
                                       StructureBuilder &structure,
                                       bool isMustHave) const {
        structure.append(isMustHave, description.value_or(L"布尔值"));
    }

    bool NodeBoolean::collectColor(const ASTNode *astNode,
                                   ColoredString &coloredString,
                                   const Theme &theme) const {
        coloredString.setColor(astNode->tokens, theme.colorBoolean);
        return true;
    }

    CODEC_NODE(NodeBoolean, descriptionTrue, descriptionFalse)

}// namespace CHelper::Node