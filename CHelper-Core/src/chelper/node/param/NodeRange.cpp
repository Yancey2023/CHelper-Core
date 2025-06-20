//
// Created by Yancey on 2024/2/27.
//

#include <chelper/node/param/NodeRange.h>
#include <chelper/node/param/NodeText.h>

namespace CHelper::Node {

    NodeRange::NodeRange(const std::optional<std::string> &id,
                         const std::optional<std::u16string> &description)
        : NodeSerializable(id, description, false) {}

    NodeTypeId::NodeTypeId NodeRange::getNodeType() const {
        return NodeTypeId::RANGE;
    }

    std::shared_ptr<ErrorReason> checkNumber(const TokensView &tokens, std::u16string_view str) {
        if (HEDLEY_UNLIKELY(str.empty())) {
            return ErrorReason::contentError(tokens, u"范围的数值为空");
        }
        for (size_t i = 0; i < str.length(); ++i) {
            size_t ch = str[i];
            if (HEDLEY_UNLIKELY(ch < '0' || ch > '9') && (i != 0 || (ch != '-' && ch != '+'))) {
                return ErrorReason::contentError(tokens, u"范围的数值格式不正确，检测非法字符");
            }
        }
        return nullptr;
    }

    ASTNode NodeRange::getASTNode(TokenReader &tokenReader, const CPack *cpack) const {
        ASTNode result = tokenReader.readStringASTNode(this);
        std::u16string_view str = result.tokens.toString();
        std::shared_ptr<ErrorReason> errorReason;
        size_t index = str.find(u"..");
        if (HEDLEY_LIKELY(index == std::u16string::npos)) {
            errorReason = checkNumber(result.tokens, str);
        } else if (HEDLEY_UNLIKELY(index == 0)) {
            errorReason = checkNumber(result.tokens, std::u16string_view(str).substr(2));
        } else {
            errorReason = checkNumber(result.tokens, std::u16string_view(str).substr(0, index));
            if (HEDLEY_UNLIKELY(errorReason == nullptr && index + 2 < str.length())) {
                errorReason = checkNumber(result.tokens, std::u16string_view(str).substr(index + 2));
            }
        }
        return ASTNode::simpleNode(this, result.tokens, errorReason);
    }

    void NodeRange::collectStructure(const ASTNode *astNode, StructureBuilder &structure, bool isMustHave) const {
        structure.append(isMustHave, description.value_or(u"范围"));
    }

    bool NodeRange::collectSyntax(const ASTNode *astNode,
                                  SyntaxResult &syntaxResult) const {
        std::u16string_view str = astNode->tokens.toString();
        for (size_t i = 0; i < str.length(); ++i) {
            size_t ch = str[i];
            syntaxResult.update(
                    astNode->tokens.startIndex + i,
                    (ch < '0' || ch > '9') && ch != '-' && ch != '+' ? SyntaxTokenType::RANGE : SyntaxTokenType::INTEGER);
        }
        return true;
    }

}// namespace CHelper::Node