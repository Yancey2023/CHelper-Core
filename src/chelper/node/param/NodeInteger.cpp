//
// Created by Yancey on 2023/11/11.
//

#include <climits>
#include "NodeInteger.h"
#include "../../util/TokenUtil.h"

namespace CHelper::Node {

    NodeInteger::NodeInteger(const std::optional<std::string> &id,
                             const std::optional<std::string> &description,
                             const std::optional<int> &min,
                             const std::optional<int> &max)
            : NodeBase(id, description, false),
              min(min),
              max(max) {}

    NodeInteger::NodeInteger(const nlohmann::json &j,
                             [[maybe_unused]] const CPack &cpack)
            : NodeBase(j, true),
              min(FROM_JSON_OPTIONAL(j, min, int)),
              max(FROM_JSON_OPTIONAL(j, max, int)) {}

    NodeType* NodeInteger::getNodeType() const {
        return NodeType::INTEGER.get();
    }

    void NodeInteger::toJson(nlohmann::json &j) const {
        NodeBase::toJson(j);
        TO_JSON_OPTIONAL(j, min)
        TO_JSON_OPTIONAL(j, max)
    }

    ASTNode NodeInteger::getASTNode(TokenReader &tokenReader, const CPack *cpack) const {
        return tokenReader.readIntegerASTNode(this);
    }

    static std::optional<int> str2int(const std::string &string) {
        if (string.length() > 11) {
            return std::nullopt;
        }
        long long result;
        std::stringstream stringStream;
        stringStream << string;
        stringStream >> result;
        stringStream.clear();
        if (result < INT_MIN || result > INT_MAX) {
            return std::nullopt;
        }
        return static_cast<int>(result);
    }

    bool NodeInteger::collectIdError(const ASTNode *astNode,
                                     std::vector<std::shared_ptr<ErrorReason>> &idErrorReasons) const {
        if (astNode->isError()) {
            return true;
        }
        std::string str = TokenUtil::toString(astNode->tokens);
        std::optional<int> num = str2int(str);
        if (!num.has_value() || (min.has_value() && num.value() < min) || (max.has_value() && num.value() > max)) {
            idErrorReasons.push_back(ErrorReason::idError(astNode->tokens, std::string("数值不在范围")
                    .append("[").append(std::to_string(min.value_or(INT_MIN)))
                    .append(", ").append(std::to_string(min.value_or(INT_MAX)))
                    .append("]").append("内 -> ").append(str)));
        }
        return true;
    }

    void NodeInteger::collectStructure(const ASTNode *astNode,
                                       StructureBuilder &structure,
                                       bool isMustHave) const {
        structure.append(isMustHave, description.value_or("整数"));
    }

} // CHelper::Node

