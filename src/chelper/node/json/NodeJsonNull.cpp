//
// Created by Yancey on 2024/2/28.
//

#include "NodeJsonNull.h"

namespace CHelper::Node {

    NodeJsonNull::NodeJsonNull(const std::optional<std::wstring> &id,
                               const std::optional<std::wstring> &description)
        : NodeBase(id, description, false) {}

    NodeType *NodeJsonNull::getNodeType() const {
        return NodeType::JSON_NULL.get();
    }

    ASTNode NodeJsonNull::getASTNode(TokenReader &tokenReader, const CPack *cpack) const {
        tokenReader.push();
        auto result = tokenReader.readStringASTNode(this);
        tokenReader.pop();
        std::wstring_view str = result.tokens.toString();
        if (HEDLEY_LIKELY(str.empty())) {
            TokensView tokens = result.tokens;
            return ASTNode::andNode(this, {std::move(result)}, tokens, ErrorReason::contentError(tokens, L"null参数为空"));
        } else if (HEDLEY_LIKELY(str != L"null")) {
            TokensView tokens = result.tokens;
            return ASTNode::andNode(this, {std::move(result)}, tokens, ErrorReason::contentError(tokens, L"内容不是null -> " + std::wstring(str)));
        }
        return result;
    }

    bool NodeJsonNull::collectSuggestions(const ASTNode *astNode,
                                          size_t index,
                                          std::vector<Suggestions> &suggestions) const {
        std::wstring_view str = astNode->tokens.toString().substr(0, index - astNode->tokens.getStartIndex());
        if (HEDLEY_LIKELY(str.find(L"null") != std::wstring::npos)) {
            std::shared_ptr<NormalId> id;
            id->name = L"null";
            id->description = L"null参数";
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