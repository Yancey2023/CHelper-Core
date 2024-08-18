//
// Created by Yancey on 2024/2/27.
//

#include "NodeRange.h"
#include "NodeText.h"

namespace CHelper::Node {

    static std::shared_ptr<NormalId> rangeSymbol = NormalId::make(L"..", L"范围");

    NodeRange::NodeRange(const std::optional<std::wstring> &id,
                         const std::optional<std::wstring> &description)
        : NodeBase(id, description, false) {}

    NodeType *NodeRange::getNodeType() const {
        return NodeType::RANGE.get();
    }

    std::shared_ptr<ErrorReason> checkNumber(const TokensView &tokens, std::wstring_view str) {
        if (HEDLEY_UNLIKELY(str.empty())) {
            return ErrorReason::contentError(tokens, L"范围的数值为空");
        }
        for (int i = 0; i < str.length(); ++i) {
            size_t ch = str[i];
            if (HEDLEY_UNLIKELY(ch < '0' || ch > '9') && (i != 0 || (ch != '-' && ch != '+'))) {
                return ErrorReason::contentError(tokens, L"范围的数值格式不正确，检测非法字符");
            }
        }
        return nullptr;
    }

    ASTNode NodeRange::getASTNode(TokenReader &tokenReader, const CPack *cpack) const {
        ASTNode result = tokenReader.readStringASTNode(this);
        std::wstring_view str = result.tokens.toString();
        std::shared_ptr<ErrorReason> errorReason;
        size_t index = str.find(L"..");
        if (HEDLEY_LIKELY(index == std::wstring::npos)) {
            errorReason = checkNumber(result.tokens, str);
        } else if (HEDLEY_UNLIKELY(index == 0)) {
            errorReason = checkNumber(result.tokens, std::wstring_view(str).substr(2));
        } else {
            errorReason = checkNumber(result.tokens, std::wstring_view(str).substr(0, index));
            if (HEDLEY_UNLIKELY(errorReason == nullptr && index + 2 < str.length())) {
                errorReason = checkNumber(result.tokens, std::wstring_view(str).substr(index + 2));
            }
        }
        return ASTNode::simpleNode(this, result.tokens, errorReason);
    }

    bool NodeRange::collectSuggestions(const ASTNode *astNode,
                                       size_t index,
                                       std::vector<Suggestions> &suggestions) const {
        std::wstring_view str = astNode->tokens.toString();
        size_t index0 = str.find(L"..");
        if (HEDLEY_UNLIKELY(index0 != std::wstring::npos)) {
            index0 += astNode->tokens.getStartIndex();
            if (HEDLEY_LIKELY(index != index0 && index != index0 + 1 && index != index0 + 2)) {
                return true;
            }
            suggestions.push_back(Suggestions::singleSymbolSuggestion({index0, index0 + 2, false, rangeSymbol}));
            return true;
        }
        size_t index1 = str.find('.');
        if (HEDLEY_UNLIKELY(index1 != std::wstring::npos)) {
            index1 += astNode->tokens.getStartIndex();
            if (HEDLEY_LIKELY(index != index1 && index != index1 + 1)) {
                return true;
            }
            suggestions.push_back(Suggestions::singleSymbolSuggestion({index0, index0 + 1, false, rangeSymbol}));
            return true;
        }
        suggestions.push_back(Suggestions::singleSymbolSuggestion({index, index, false, rangeSymbol}));
        return true;
    }

    void NodeRange::collectStructure(const ASTNode *astNode, StructureBuilder &structure, bool isMustHave) const {
        structure.append(isMustHave, description.value_or(L"范围"));
    }

    bool NodeRange::collectColor(const ASTNode *astNode,
                                 ColoredString &coloredString,
                                 const Theme &theme) const {
        std::wstring_view str = astNode->tokens.toString();
        for (int i = 0; i < str.length(); ++i) {
            size_t ch = str[i];
            coloredString.setColor(
                    astNode->tokens.startIndex + i,
                    (ch < '0' || ch > '9') && ch != '-' && ch != '+' ? theme.colorRange : theme.colorInteger);
        }
        return true;
    }

    CODEC_UNIQUE_PTR(NodeRange)

}// namespace CHelper::Node