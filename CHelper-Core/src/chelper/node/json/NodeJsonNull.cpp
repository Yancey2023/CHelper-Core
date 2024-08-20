//
// Created by Yancey on 2024/2/28.
//

#include <chelper/node/json/NodeJsonNull.h>

namespace CHelper::Node {

    NodeJsonNull::NodeJsonNull(const std::optional<std::u16string> &id,
                               const std::optional<std::u16string> &description)
        : NodeBase(id, description, false) {}

    NodeType *NodeJsonNull::getNodeType() const {
        return NodeType::JSON_NULL.get();
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
            return ASTNode::andNode(this, {std::move(result)}, tokens, ErrorReason::contentError(tokens, u"内容不是null -> " + std::u16string(str)));
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

    bool NodeJsonNull::collectColor(const ASTNode *astNode,
                                    ColoredString &coloredString,
                                    const Theme &theme) const {
        coloredString.setColor(astNode->tokens, theme.colorNull);
        return true;
    }

    CODEC_UNIQUE_PTR(NodeJsonNull)

}// namespace CHelper::Node