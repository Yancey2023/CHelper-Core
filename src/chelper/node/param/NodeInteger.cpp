//
// Created by Yancey on 2023/11/11.
//

#include "NodeInteger.h"

namespace CHelper::Node {

    NodeInteger::NodeInteger(const std::optional<std::wstring> &id,
                             const std::optional<std::wstring> &description,
                             const std::optional<int32_t> &min,
                             const std::optional<int32_t> &max)
        : NodeBase(id, description, false),
          min(min),
          max(max) {}

    NodeType *NodeInteger::getNodeType() const {
        return NodeType::INTEGER.get();
    }

    ASTNode NodeInteger::getASTNode(TokenReader &tokenReader, const CPack *cpack) const {
        return tokenReader.readIntegerASTNode(this);
    }

    bool NodeInteger::collectIdError(const ASTNode *astNode,
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

    void NodeInteger::collectStructure(const ASTNode *astNode,
                                       StructureBuilder &structure,
                                       bool isMustHave) const {
        structure.append(isMustHave, description.value_or(L"整数"));
    }

    bool NodeInteger::collectColor(const ASTNode *astNode,
                                   ColoredString &coloredString,
                                   const Theme &theme) const {
        coloredString.setColor(astNode->tokens, theme.colorInteger);
        return true;
    }

    CODEC_NODE(NodeInteger, min, max)

}// namespace CHelper::Node
