//
// Created by Yancey on 2023/11/11.
//

#include "NodeInteger.h"
#include "../../util/TokenUtil.h"

namespace CHelper::Node {

    NodeInteger::NodeInteger(const std::optional<std::string> &id,
                             const std::optional<std::string> &description,
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
        std::string str = TokenUtil::toString(astNode->tokens);
        char *end;
        std::intmax_t value = std::strtoimax(str.c_str(), &end, 10);
        if (HEDLEY_UNLIKELY(end == str.c_str() || *end != '\0' ||
                            value == HUGE_VALF || value == -HUGE_VALF ||
                            value < min.value_or(std::numeric_limits<int32_t>::lowest()) ||
                            value > max.value_or(std::numeric_limits<int32_t>::max()))) {
            idErrorReasons.push_back(ErrorReason::idError(astNode->tokens, std::string("数值不在范围")
                                                                                   .append("[")
                                                                                   .append(std::to_string(min.value_or(std::numeric_limits<int32_t>::lowest())))
                                                                                   .append(", ")
                                                                                   .append(std::to_string(max.value_or(std::numeric_limits<int32_t>::max())))
                                                                                   .append("]")
                                                                                   .append("内 -> ")
                                                                                   .append(str)));
        }
        return true;
    }

    void NodeInteger::collectStructure(const ASTNode *astNode,
                                       StructureBuilder &structure,
                                       bool isMustHave) const {
        structure.append(isMustHave, description.value_or("整数"));
    }

    CODEC_NODE(NodeInteger, min, max)

}// namespace CHelper::Node
