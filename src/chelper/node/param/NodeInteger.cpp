//
// Created by Yancey on 2023/11/11.
//

#include "NodeInteger.h"

namespace CHelper::Node {

    NodeInteger::NodeInteger(const std::optional<std::string> &id,
                             const std::optional<std::string> &description,
                             const std::optional<int> &min,
                             const std::optional<int> &max)
            : NodeBase(id, description, false),
              min(min),
              max(max) {}

    NodeInteger::NodeInteger(const nlohmann::json &j,
                             const CPack &cpack)
            : NodeBase(j),
              min(FROM_JSON_OPTIONAL(j, min, int)),
              max(FROM_JSON_OPTIONAL(j, max, int)) {}

    NodeType NodeInteger::getNodeType() const {
        return NodeType::INTEGER;
    }

    void NodeInteger::toJson(nlohmann::json &j) const {
        NodeBase::toJson(j);
        TO_JSON_OPTIONAL(j, min)
        TO_JSON_OPTIONAL(j, max)
    }

    ASTNode NodeInteger::getASTNode(TokenReader &tokenReader) const {
        return tokenReader.readIntegerASTNode(this);
    }

    void NodeInteger::collectStructure(const ASTNode *astNode,
                                       StructureBuilder &structure,
                                       bool isMustHave) const {
        structure.append(isMustHave, description.value_or("整数"));
    }

} // CHelper::Node

