//
// Created by Yancey on 2023/11/11.
//

#include "NodeBoolean.h"

namespace CHelper::Node {

    NodeBoolean::NodeBoolean(const std::optional<std::string> &id,
                             const std::optional<std::string> &description,
                             const std::optional<std::string> &descriptionTrue,
                             const std::optional<std::string> &descriptionFalse)
        : NodeBase(id, description, false),
          descriptionTrue(descriptionTrue),
          descriptionFalse(descriptionFalse) {}

    NodeType *NodeBoolean::getNodeType() const {
        return NodeType::BOOLEAN.get();
    }

    ASTNode NodeBoolean::getASTNode(TokenReader &tokenReader, const CPack *cpack) const {
        ASTNode astNode = tokenReader.readStringASTNode(this);
        std::string_view str = astNode.tokens.toString();
        if (HEDLEY_LIKELY(str == "true" || str == "false")) {
            return astNode;
        }
        TokensView tokens = astNode.tokens;
        return ASTNode::andNode(this, {std::move(astNode)}, tokens, ErrorReason::contentError(tokens, "内容不匹配，应该为布尔值，但当前内容为" + std::string(str)));
    }

    bool NodeBoolean::collectSuggestions(const ASTNode *astNode,
                                         size_t index,
                                         std::vector<Suggestions> &suggestions) const {
        KMPMatcher kmpMatcher(astNode->tokens.toString().substr(0, index - astNode->tokens.getStartIndex()));
        Suggestions suggestions1;
        if (HEDLEY_UNLIKELY(kmpMatcher.match("true") != std::string::npos)) {
            suggestions1.suggestions.emplace_back(
                    astNode->tokens, true,
                    NormalId::make("true", descriptionTrue));
        }
        if (HEDLEY_UNLIKELY(kmpMatcher.match("false") != std::string::npos)) {
            suggestions1.suggestions.emplace_back(
                    astNode->tokens, true,
                    NormalId::make("false", descriptionFalse));
        }
        suggestions1.markFiltered();
        suggestions.push_back(std::move(suggestions1));
        return true;
    }

    void NodeBoolean::collectStructure(const ASTNode *astNode,
                                       StructureBuilder &structure,
                                       bool isMustHave) const {
        structure.append(isMustHave, description.value_or("布尔值"));
    }

    CODEC_NODE(NodeBoolean, descriptionTrue, descriptionFalse)

}// namespace CHelper::Node