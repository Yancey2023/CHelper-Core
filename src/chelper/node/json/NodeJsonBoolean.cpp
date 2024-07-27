//
// Created by Yancey on 2024/2/28.
//

#include "NodeJsonBoolean.h"

namespace CHelper::Node {

    NodeJsonBoolean::NodeJsonBoolean(const std::optional<std::string> &id,
                                     const std::optional<std::string> &description,
                                     const std::optional<std::string> &descriptionTrue,
                                     const std::optional<std::string> &descriptionFalse)
        : NodeBase(id, description, false),
          descriptionTrue(descriptionTrue),
          descriptionFalse(descriptionFalse) {}

    NodeType *NodeJsonBoolean::getNodeType() const {
        return NodeType::JSON_BOOLEAN.get();
    }

    ASTNode NodeJsonBoolean::getASTNode(TokenReader &tokenReader, const CPack *cpack) const {
        ASTNode astNode = tokenReader.readStringASTNode(this);
        std::string_view str = astNode.tokens.toString();
        if (HEDLEY_UNLIKELY(str == "true" || str == "false")) {
            return astNode;
        }
        TokensView tokens = astNode.tokens;
        return ASTNode::andNode(this, {std::move(astNode)}, tokens, ErrorReason::contentError(tokens, "内容不匹配，应该为布尔值，但当前内容为" + std::string(str)));
    }

    bool NodeJsonBoolean::collectSuggestions(const ASTNode *astNode,
                                             size_t index,
                                             std::vector<Suggestions> &suggestions) const {
        KMPMatcher kmpMatcher(astNode->tokens.toString().substr(0, index - astNode->tokens.getStartIndex()));
        Suggestions suggestions1;
        if (HEDLEY_UNLIKELY(kmpMatcher.match("true") != std::string::npos)) {
            std::shared_ptr<NormalId> id;
            id->name = "true";
            id->description = descriptionTrue;
            suggestions1.suggestions.emplace_back(astNode->tokens, false, std::move(id));
        }
        if (HEDLEY_UNLIKELY(kmpMatcher.match("false") != std::string::npos)) {
            std::shared_ptr<NormalId> id;
            id->name = "false";
            id->description = descriptionFalse;
            suggestions1.suggestions.emplace_back(astNode->tokens, false, std::move(id));
        }
        suggestions1.markFiltered();
        suggestions.push_back(std::move(suggestions1));
        return true;
    }

    CODEC_NODE(NodeJsonBoolean, descriptionTrue, descriptionFalse)

}// namespace CHelper::Node