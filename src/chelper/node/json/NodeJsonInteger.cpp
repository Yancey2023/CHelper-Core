//
// Created by Yancey on 2024/2/28.
//

#include "NodeJsonInteger.h"
#include "../../util/TokenUtil.h"

namespace CHelper::Node {

    NodeJsonInteger::NodeJsonInteger(const std::optional<std::string> &id,
                                     const std::optional<std::string> &description,
                                     const std::optional<int> &min,
                                     const std::optional<int> &max)
            : NodeBase(id, description, false),
              min(min),
              max(max) {}

    NodeJsonInteger::NodeJsonInteger(const nlohmann::json &j,
                                     [[maybe_unused]] const CPack &cpack)
            : NodeBase(j, false),
              min(FROM_JSON_OPTIONAL(j, min, int)),
              max(FROM_JSON_OPTIONAL(j, max, int)) {}

    std::shared_ptr<NodeType> NodeJsonInteger::getNodeType() const {
        return NodeType::JSON_INTEGER;
    }

    void NodeJsonInteger::toJson(nlohmann::json &j) const {
        NodeBase::toJson(j);
        TO_JSON_OPTIONAL(j, min)
        TO_JSON_OPTIONAL(j, max)
    }

    ASTNode NodeJsonInteger::getASTNode(TokenReader &tokenReader, const CPack &cpack) const {
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
        if (result < -2147483648 || result > 2147483647) {
            return std::nullopt;
        }
        return static_cast<int>(result);
    }

    bool NodeJsonInteger::collectIdError(const ASTNode *astNode,
                                         std::vector<std::shared_ptr<ErrorReason>> &idErrorReasons) const {
        if (astNode->isError()) {
            return true;
        }
        std::string str = TokenUtil::toString(astNode->tokens);
        std::optional<int> num = str2int(str);
        if (!num.has_value() || (min.has_value() && num.value() < min) || (max.has_value() && num.value() > max)) {
            idErrorReasons.push_back(ErrorReason::idError(astNode->tokens, std::string("数值不在范围")
                    .append("[").append(std::to_string(min.value_or(-2147483648)))
                    .append(", ").append(std::to_string(min.value_or(2147483647)))
                    .append("]").append("内 -> ").append(str)));
        }
        return true;
    }

} // CHelper::Node

