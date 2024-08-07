//
// Created by Yancey on 2023/11/11.
//

#include "NodeFloat.h"

namespace CHelper::Node {

    NodeFloat::NodeFloat(const std::optional<std::string> &id,
                         const std::optional<std::string> &description,
                         const std::optional<float> &min,
                         const std::optional<float> &max)
        : NodeBase(id, description, false),
          min(min),
          max(max) {}

    NodeType *NodeFloat::getNodeType() const {
        return NodeType::FLOAT.get();
    }

    ASTNode NodeFloat::getASTNode(TokenReader &tokenReader, const CPack *cpack) const {
        return tokenReader.readFloatASTNode(this);
    }

    bool NodeFloat::collectIdError(const ASTNode *astNode,
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

    void NodeFloat::collectStructure(const ASTNode *astNode,
                                     StructureBuilder &structure,
                                     bool isMustHave) const {
        structure.append(isMustHave, description.value_or("数字"));
    }

    bool NodeFloat::collectColor(const ASTNode *astNode,
                                 ColoredString &coloredString,
                                 const Theme &theme) const {
        coloredString.setColor(astNode->tokens, theme.colorFloat);
        return true;
    }

    CODEC_NODE(NodeFloat, min, max)

}// namespace CHelper::Node
