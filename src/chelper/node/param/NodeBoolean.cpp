//
// Created by Yancey on 2023/11/11.
//

#include "NodeBoolean.h"

namespace CHelper::Node {

    NodeBoolean::NodeBoolean(const std::optional<std::string> &id,
                             const std::optional<std::string> &description,
                             const std::optional<std::string> &descriptionTrue,
                             const std::optional<std::string> &descriptionFalse)
            : NodeBase(id, description, false),
              descriptionTrue(descriptionTrue),
              descriptionFalse(descriptionFalse) {}

    NodeBoolean::NodeBoolean(const nlohmann::json &j,
                             const CPack &cpack)
            : NodeBase(j),
              descriptionTrue(FROM_JSON_OPTIONAL(j, descriptionTrue, std::string)),
              descriptionFalse(FROM_JSON_OPTIONAL(j, descriptionFalse, std::string)) {}

    NodeType NodeBoolean::getNodeType() const {
        return NodeType::BOOLEAN;
    }

    void NodeBoolean::toJson(nlohmann::json &j) const {
        NodeBase::toJson(j);
        TO_JSON_OPTIONAL(j, descriptionTrue)
        TO_JSON_OPTIONAL(j, descriptionFalse)
    }

    ASTNode NodeBoolean::getASTNode(TokenReader &tokenReader) const {
        return tokenReader.getStringASTNode(this);
    }

    void NodeBoolean::collectStructure(const ASTNode *astNode,
                                       StructureBuilder &structure,
                                       bool isMustHave) const {
        structure.append(isMustHave, description.value_or("布尔值"));
    }

} // CHelper::Node