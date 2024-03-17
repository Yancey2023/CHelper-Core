//
// Created by Yancey on 2024/2/28.
//

#include "NodeJsonInteger.h"
#include "../../util/TokenUtil.h"

namespace CHelper::Node {

    NodeJsonInteger::NodeJsonInteger(const std::optional<std::string> &id,
                                     const std::optional<std::string> &description,
                                     const std::optional<std::int32_t> &min,
                                     const std::optional<std::int32_t> &max)
            : NodeBase(id, description, false),
              min(min),
              max(max) {}

    NodeJsonInteger::NodeJsonInteger(const nlohmann::json &j,
                                     [[maybe_unused]] const CPack &cpack)
            : NodeBase(j, false),
              min(JsonUtil::fromJsonOptionalUnlikely<std::int32_t>(j, "min")),
              max(JsonUtil::fromJsonOptionalUnlikely<std::int32_t>(j, "max")) {}

    NodeType *NodeJsonInteger::getNodeType() const {
        return NodeType::JSON_INTEGER.get();
    }

    void NodeJsonInteger::toJson(nlohmann::json &j) const {
        NodeBase::toJson(j);
        JsonUtil::toJsonOptionalUnlikely(j, "min", min);
        JsonUtil::toJsonOptionalUnlikely(j, "max", max);
    }

    ASTNode NodeJsonInteger::getASTNode(TokenReader &tokenReader, const CPack *cpack) const {
        return tokenReader.readIntegerASTNode(this);
    }

    static std::optional<std::int32_t> str2int(const std::string &string) {
        if (HEDLEY_UNLIKELY(string.length() > 11)) {
            return std::nullopt;
        }
        std::int64_t result;
        std::stringstream stringStream;
        stringStream << string;
        stringStream >> result;
        stringStream.clear();
        if (HEDLEY_UNLIKELY(result < INT32_MIN || result > INT32_MAX)) {
            return std::nullopt;
        }
        return static_cast<std::int32_t>(result);
    }

    bool NodeJsonInteger::collectIdError(const ASTNode *astNode,
                                         std::vector<std::shared_ptr<ErrorReason>> &idErrorReasons) const {
        if (HEDLEY_UNLIKELY(astNode->isError())) {
            return true;
        }
        std::string str = TokenUtil::toString(astNode->tokens);
        std::optional<std::int32_t> num = str2int(str);
        if (HEDLEY_UNLIKELY(!num.has_value() ||
                            (min.has_value() && num.value() < min) ||
                            (max.has_value() && num.value() > max))) {
            idErrorReasons.push_back(ErrorReason::idError(astNode->tokens, std::string("数值不在范围")
                    .append("[").append(std::to_string(min.value_or(INT32_MIN)))
                    .append(", ").append(std::to_string(max.value_or(INT32_MAX)))
                    .append("]").append("内 -> ").append(str)));
        }
        return true;
    }

} // CHelper::Node

