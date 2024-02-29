//
// Created by Yancey on 2024/2/28.
//

#include "NodeJsonFloat.h"
#include "../../util/TokenUtil.h"

namespace CHelper::Node {

    NodeJsonFloat::NodeJsonFloat(const std::optional<std::string> &id,
                                 const std::optional<std::string> &description,
                                 const std::optional<float> &min,
                                 const std::optional<float> &max)
            : NodeBase(id, description, false),
              min(min),
              max(max) {}

    NodeJsonFloat::NodeJsonFloat(const nlohmann::json &j,
                                 [[maybe_unused]] const CPack &cpack)
            : NodeBase(j, false),
              min(FROM_JSON_OPTIONAL(j, min, float)),
              max(FROM_JSON_OPTIONAL(j, max, float)) {}

    std::shared_ptr<NodeType> NodeJsonFloat::getNodeType() const {
        return NodeType::JSON_FLOAT;
    }

    void NodeJsonFloat::toJson(nlohmann::json &j) const {
        NodeBase::toJson(j);
        TO_JSON_OPTIONAL(j, min)
        TO_JSON_OPTIONAL(j, max)
    }

    ASTNode NodeJsonFloat::getASTNode(TokenReader &tokenReader, const CPack &cpack) const {
        return tokenReader.readFloatASTNode(this);
    }

    static std::optional<float> str2float(const std::string &string) {
        //TODO 需要更好的字符串转小数
        double result;
        std::stringstream stringStream;
        stringStream << string;
        stringStream >> std::setprecision(16) >> result;
        stringStream.clear();
        return result;
    }

    bool NodeJsonFloat::collectIdError(const ASTNode *astNode,
                                       std::vector<std::shared_ptr<ErrorReason>> &idErrorReasons) const {
        if (astNode->isError()) {
            return true;
        }
        std::string str = TokenUtil::toString(astNode->tokens);
        std::optional<float> num = str2float(str);
        if (!num.has_value() || (min.has_value() && num.value() < min) || (max.has_value() && num.value() > max)) {
            idErrorReasons.push_back(ErrorReason::idError(astNode->tokens, std::string("数值不在范围")
                    .append("[").append(min.has_value() ? std::to_string(min.value()) : "-3.4028235E38")
                    .append(", ").append(max.has_value() ? std::to_string(max.value()) : "3.4028235E38")
                    .append("]").append("内 -> ").append(str)));
        }
        return true;
    }

} // CHelper::Node