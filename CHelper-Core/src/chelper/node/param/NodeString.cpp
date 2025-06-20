//
// Created by Yancey on 2023/12/10.
//

#include <chelper/node/param/NodeString.h>

namespace CHelper::Node {

    NodeString::NodeString(const std::optional<std::string> &id,
                           const std::optional<std::u16string> &description,
                           bool allowMissingString,
                           const bool canContainSpace,
                           const bool ignoreLater)
        : NodeSerializable(id, description, false),
          allowMissingString(allowMissingString),
          canContainSpace(canContainSpace),
          ignoreLater(ignoreLater) {}

    NodeTypeId::NodeTypeId NodeString::getNodeType() const {
        return NodeTypeId::STRING;
    }

    ASTNode NodeString::getASTNode(TokenReader &tokenReader, const CPack *cpack) const {
        if (HEDLEY_UNLIKELY(ignoreLater)) {
            //后面的所有内容都算作这个字符串
            tokenReader.push();
            tokenReader.skipToLF();
            TokensView tokens = tokenReader.collect();
            if (HEDLEY_UNLIKELY(!allowMissingString && tokens.isEmpty())) {
                return ASTNode::simpleNode(this, tokens, ErrorReason::incomplete(tokens, u"字符串参数内容为空"));
            } else {
                return ASTNode::simpleNode(this, tokens);
            }
        }
        tokenReader.push();
        ASTNode result = tokenReader.readStringASTNode(this);
        if (HEDLEY_UNLIKELY(allowMissingString && result.isError())) {
            tokenReader.restore();
            tokenReader.push();
            return ASTNode::simpleNode(this, tokenReader.collect());
        }
        tokenReader.pop();
        if (HEDLEY_UNLIKELY(!allowMissingString && result.tokens.isEmpty())) {
            return ASTNode::simpleNode(this, result.tokens, ErrorReason::incomplete(result.tokens, u"字符串参数内容为空"));
        }
        if (HEDLEY_UNLIKELY(!canContainSpace)) {
            if (HEDLEY_UNLIKELY(result.tokens.toString().find(' ') != std::u16string::npos)) {
                return ASTNode::simpleNode(this, result.tokens, ErrorReason::contentError(result.tokens, u"字符串参数内容不可以包含空格"));
            }
            return result;
        }
        std::u16string_view str = result.tokens.toString();
        if (HEDLEY_LIKELY(str.empty() || str[0] != '"')) {
            return result;
        }
        auto convertResult = JsonUtil::jsonString2String(std::u16string(str));
        size_t offset = result.tokens.getStartIndex();
        if (HEDLEY_UNLIKELY(convertResult.errorReason != nullptr)) {
            convertResult.errorReason->start += offset;
            convertResult.errorReason->end += offset;
            return ASTNode::simpleNode(this, result.tokens, convertResult.errorReason);
        }
        if (HEDLEY_UNLIKELY(!convertResult.isComplete)) {
            return ASTNode::simpleNode(this, result.tokens, ErrorReason::contentError(result.tokens, fmt::format(u"字符串参数内容双引号不封闭 -> {}", str)));
        }
        return result;
    }

}// namespace CHelper::Node