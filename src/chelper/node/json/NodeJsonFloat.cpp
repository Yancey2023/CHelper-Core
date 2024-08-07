//
// Created by Yancey on 2024/2/28.
//

#include "NodeJsonFloat.h"

namespace CHelper::Node {

    NodeJsonFloat::NodeJsonFloat(const std::optional<std::string> &id,
                                 const std::optional<std::string> &description,
                                 const std::optional<float> &min,
                                 const std::optional<float> &max)
        : NodeBase(id, description, false),
          min(min),
          max(max) {}

    NodeType *NodeJsonFloat::getNodeType() const {
        return NodeType::JSON_FLOAT.get();
    }

    ASTNode NodeJsonFloat::getASTNode(TokenReader &tokenReader, const CPack *cpack) const {
        return tokenReader.readFloatASTNode(this);
    }

    bool NodeJsonFloat::collectIdError(const ASTNode *astNode,
                                       std::vector<std::shared_ptr<ErrorReason>> &idErrorReasons) const {
        if (HEDLEY_UNLIKELY(astNode->isError())) {
            return true;
        }
        std::string str(astNode->tokens.toString());
        char *end;
        float value = std::strtof(str.c_str(), &end);
        if (HEDLEY_UNLIKELY(end == str.c_str() || *end != '\0' ||
                            value == HUGE_VALF || value == -HUGE_VALF ||
                            (min.has_value() && value < min) ||
                            (max.has_value() && value > max))) {
            idErrorReasons.push_back(ErrorReason::idError(astNode->tokens, std::string("数值不在范围")
                                                                                   .append("[")
                                                                                   .append(std::to_string(min.value_or(std::numeric_limits<float>::lowest())))
                                                                                   .append(", ")
                                                                                   .append(std::to_string(max.value_or(std::numeric_limits<float>::max())))
                                                                                   .append("]")
                                                                                   .append("内 -> ")
                                                                                   .append(str)));
        }
        return true;
    }

    bool NodeJsonFloat::collectColor(const ASTNode *astNode,
                                     ColoredString &coloredString,
                                     const Theme &theme) const {
        coloredString.setColor(astNode->tokens, theme.colorFloat);
        return true;
    }

    CODEC_NODE(NodeJsonFloat, min, max)

}// namespace CHelper::Node