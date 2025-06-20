//
// Created by Yancey on 2024/2/28.
//

#include <chelper/node/json/NodeJsonNull.h>

namespace CHelper::Node {

    NodeJsonNull::NodeJsonNull(const std::optional<std::string> &id,
                               const std::optional<std::u16string> &description)
        : NodeSerializable(id, description, false) {}

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

}// namespace CHelper::Node