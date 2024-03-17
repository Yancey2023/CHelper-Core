//
// Created by Yancey on 2024/2/28.
//

#include <cfloat>
#include "NodeJsonFloat.h"
#include "../../util/TokenUtil.h"

namespace CHelper::Node {

    NodeJsonFloat::NodeJsonFloat(const std::optional<std::string> &id,
                                 const std::optional<std::string> &description,
                                 const std::optional<std::float_t> &min,
                                 const std::optional<std::float_t> &max)
            : NodeBase(id, description, false),
              min(min),
              max(max) {}

    NodeJsonFloat::NodeJsonFloat(const nlohmann::json &j,
                                 [[maybe_unused]] const CPack &cpack)
            : NodeBase(j, false),
              min(JsonUtil::fromJsonOptionalUnlikely<std::float_t>(j, "min")),
              max(JsonUtil::fromJsonOptionalUnlikely<std::float_t>(j, "max")) {}

    NodeType *NodeJsonFloat::getNodeType() const {
        return NodeType::JSON_FLOAT.get();
    }

    void NodeJsonFloat::toJson(nlohmann::json &j) const {
        NodeBase::toJson(j);
        JsonUtil::toJsonOptionalUnlikely(j, "min", min);
        JsonUtil::toJsonOptionalUnlikely(j, "max", max);
    }

    ASTNode NodeJsonFloat::getASTNode(TokenReader &tokenReader, const CPack *cpack) const {
        return tokenReader.readFloatASTNode(this);
    }

    static std::optional<std::float_t> str2float(const std::string &string) {
        std::double_t result;
        std::stringstream stringStream;
        stringStream << string;
        stringStream >> std::setprecision(16) >> result;
        stringStream.clear();
        if (HEDLEY_UNLIKELY(result < FLT_MIN || result > FLT_MAX)) {
            return std::nullopt;
        }
        return static_cast<std::float_t>(result);
    }

    bool NodeJsonFloat::collectIdError(const ASTNode *astNode,
                                       std::vector<std::shared_ptr<ErrorReason>> &idErrorReasons) const {
        if (HEDLEY_UNLIKELY(astNode->isError())) {
            return true;
        }
        std::string str = TokenUtil::toString(astNode->tokens);
        std::optional<std::float_t> num = str2float(str);
        if (HEDLEY_UNLIKELY(!num.has_value() ||
                            (min.has_value() && num.value() < min) ||
                            (max.has_value() && num.value() > max))) {
            idErrorReasons.push_back(ErrorReason::idError(astNode->tokens, std::string("数值不在范围")
                    .append("[").append(std::to_string(min.value_or(FLT_MIN)))
                    .append(", ").append(std::to_string(max.value_or(FLT_MAX)))
                    .append("]").append("内 -> ").append(str)));
        }
        return true;
    }

} // CHelper::Node