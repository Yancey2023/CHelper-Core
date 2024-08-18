//
// Created by Yancey on 2024/2/28.
//

#include "NodeJsonInteger.h"

namespace CHelper::Node {

    NodeJsonInteger::NodeJsonInteger(const std::optional<std::wstring> &id,
                                     const std::optional<std::wstring> &description,
                                     const std::optional<int32_t> &min,
                                     const std::optional<int32_t> &max)
        : NodeBase(id, description, false),
          min(min),
          max(max) {}

    NodeType *NodeJsonInteger::getNodeType() const {
        return NodeType::JSON_INTEGER.get();
    }

    ASTNode NodeJsonInteger::getASTNode(TokenReader &tokenReader, const CPack *cpack) const {
        return tokenReader.readIntegerASTNode(this);
    }

    bool NodeJsonInteger::collectIdError(const ASTNode *astNode,
                                         std::vector<std::shared_ptr<ErrorReason>> &idErrorReasons) const {
        if (HEDLEY_UNLIKELY(astNode->isError())) {
            return true;
        }
        std::wstring str(astNode->tokens.toString());
        wchar_t *end;
        std::intmax_t value = std::wcstoimax(str.c_str(), &end, 10);
        if (HEDLEY_UNLIKELY(end == str.c_str() || *end != '\0' ||
                            value == HUGE_VALF || value == -HUGE_VALF ||
                            value < min.value_or(std::numeric_limits<int32_t>::lowest()) ||
                            value > max.value_or(std::numeric_limits<int32_t>::max()))) {
            idErrorReasons.push_back(ErrorReason::idError(astNode->tokens, std::wstring(L"数值不在范围")
                                                                                   .append(L"[")
                                                                                   .append(std::to_wstring(min.value_or(std::numeric_limits<int32_t>::lowest())))
                                                                                   .append(L", ")
                                                                                   .append(std::to_wstring(max.value_or(std::numeric_limits<int32_t>::max())))
                                                                                   .append(L"]")
                                                                                   .append(L"内 -> ")
                                                                                   .append(str)));
        }
        return true;
    }

    bool NodeJsonInteger::collectColor(const ASTNode *astNode,
                                       ColoredString &coloredString,
                                       const Theme &theme) const {
        coloredString.setColor(astNode->tokens, theme.colorInteger);
        return true;
    }

    CODEC_NODE(NodeJsonInteger, min, max)

}// namespace CHelper::Node
