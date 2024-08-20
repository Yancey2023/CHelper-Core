//
// Created by Yancey on 2023/11/11.
//

#include <chelper/node/param/NodeBoolean.h>

namespace CHelper::Node {

    NodeBoolean::NodeBoolean(const std::optional<std::u16string> &id,
                             const std::optional<std::u16string> &description,
                             const std::optional<std::u16string> &descriptionTrue,
                             const std::optional<std::u16string> &descriptionFalse)
        : NodeBase(id, description, false),
          descriptionTrue(descriptionTrue),
          descriptionFalse(descriptionFalse) {}

    NodeType *NodeBoolean::getNodeType() const {
        return NodeType::BOOLEAN.get();
    }

    ASTNode NodeBoolean::getASTNode(TokenReader &tokenReader, const CPack *cpack) const {
        ASTNode astNode = tokenReader.readStringASTNode(this);
        std::u16string_view str = astNode.tokens.toString();
        if (HEDLEY_LIKELY(str == u"true" || str == u"false")) {
            return astNode;
        }
        TokensView tokens = astNode.tokens;
        return ASTNode::andNode(this, {std::move(astNode)}, tokens, ErrorReason::contentError(tokens, u"内容不匹配，应该为布尔值，但当前内容为" + std::u16string(str)));
    }

    bool NodeBoolean::collectSuggestions(const ASTNode *astNode,
                                         size_t index,
                                         std::vector<Suggestions> &suggestions) const {
        KMPMatcher kmpMatcher(astNode->tokens.toString().substr(0, index - astNode->tokens.getStartIndex()));
        Suggestions suggestions1(SuggestionsType::LITERAL);
        if (HEDLEY_UNLIKELY(kmpMatcher.match(u"true") != std::u16string::npos)) {
            suggestions1.suggestions.emplace_back(
                    astNode->tokens, true,
                    NormalId::make(u"true", descriptionTrue));
        }
        if (HEDLEY_UNLIKELY(kmpMatcher.match(u"false") != std::u16string::npos)) {
            suggestions1.suggestions.emplace_back(
                    astNode->tokens, true,
                    NormalId::make(u"false", descriptionFalse));
        }
        suggestions1.markFiltered();
        suggestions.push_back(std::move(suggestions1));
        return true;
    }

    void NodeBoolean::collectStructure(const ASTNode *astNode,
                                       StructureBuilder &structure,
                                       bool isMustHave) const {
        structure.append(isMustHave, description.value_or(u"布尔值"));
    }

    bool NodeBoolean::collectColor(const ASTNode *astNode,
                                   ColoredString &coloredString,
                                   const Theme &theme) const {
        coloredString.setColor(astNode->tokens, theme.colorBoolean);
        return true;
    }

    CODEC_NODE(NodeBoolean, descriptionTrue, descriptionFalse)

}// namespace CHelper::Node