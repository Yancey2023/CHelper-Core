//
// Created by Yancey on 2024/2/28.
//

#include "NodeJsonNull.h"

namespace CHelper::Node {

    NodeJsonNull::NodeJsonNull(const std::optional<std::string> &id,
                               const std::optional<std::string> &description)
        : NodeBase(id, description, false) {}

    NodeType *NodeJsonNull::getNodeType() const {
        return NodeType::JSON_NULL.get();
    }

    ASTNode NodeJsonNull::getASTNode(TokenReader &tokenReader, const CPack *cpack) const {
        tokenReader.push();
        auto result = tokenReader.readStringASTNode(this);
        tokenReader.pop();
        std::string_view str = result.tokens.toString();
        if (HEDLEY_LIKELY(str.empty())) {
            TokensView tokens = result.tokens;
            return ASTNode::andNode(this, {std::move(result)}, tokens, ErrorReason::contentError(tokens, "null参数为空"));
        } else if (HEDLEY_LIKELY(str != "null")) {
            TokensView tokens = result.tokens;
            return ASTNode::andNode(this, {std::move(result)}, tokens, ErrorReason::contentError(tokens, "内容不是null -> " + std::string(str)));
        }
        return result;
    }

    bool NodeJsonNull::collectSuggestions(const ASTNode *astNode,
                                          size_t index,
                                          std::vector<Suggestions> &suggestions) const {
        std::string_view str = astNode->tokens.toString().substr(0, index - astNode->tokens.getStartIndex());
        if (HEDLEY_LIKELY(str.find("null") != std::string::npos)) {
            std::shared_ptr<NormalId> id;
            id->name = "null";
            id->description = "null参数";
            suggestions.push_back(Suggestions::singleSuggestion({astNode->tokens, false, id}));
        }
        return true;
    }

    CODEC_UNIQUE_PTR(NodeJsonNull)

}// namespace CHelper::Node