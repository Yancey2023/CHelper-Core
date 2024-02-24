//
// Created by Yancey on 2023/11/11.
//

#include "NodeFloat.h"

namespace CHelper::Node {

    NodeFloat::NodeFloat(const std::optional<std::string> &id,
                         const std::optional<std::string> &description,
                         const std::optional<float> &min,
                         const std::optional<float> &max)
            : NodeBase(id, description),
              min(min),
              max(max) {}

    NodeFloat::NodeFloat(const nlohmann::json &j,
                         const CPack &cpack)
            : NodeBase(j, cpack),
              min(FROM_JSON_OPTIONAL(j, min, float)),
              max(FROM_JSON_OPTIONAL(j, max, float)) {}

    NodeType NodeFloat::getNodeType() const {
        return NodeType::FLOAT;
    }

    void NodeFloat::toJson(nlohmann::json &j) const {
        NodeBase::toJson(j);
        TO_JSON_OPTIONAL(j, min)
        TO_JSON_OPTIONAL(j, max)
    }

    ASTNode NodeFloat::getASTNode(TokenReader &tokenReader, const CPack &cpack) const {
        return getFloatASTNode(tokenReader);
    }

    void NodeFloat::collectStructure(const ASTNode *astNode,
                                     StructureBuilder &structure,
                                     bool isMustHave) const {
        structure.append(isMustHave, description.value_or("数字"));
    }

} // CHelper::Node
