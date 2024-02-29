//
// Created by Yancey on 2024/2/28.
//

#include "NodeJsonBoolean.h"

namespace CHelper::Node {

    NodeJsonBoolean::NodeJsonBoolean(const std::optional<std::string> &id,
                                     const std::optional<std::string> &description,
                                     const std::optional<std::string> &descriptionTrue,
                                     const std::optional<std::string> &descriptionFalse)
            : NodeBase(id, description, false),
              descriptionTrue(descriptionTrue),
              descriptionFalse(descriptionFalse) {}

    NodeJsonBoolean::NodeJsonBoolean(const nlohmann::json &j,
                                     [[maybe_unused]] const CPack &cpack)
            : NodeBase(j, false),
              descriptionTrue(FROM_JSON_OPTIONAL(j, descriptionTrue, std::string)),
              descriptionFalse(FROM_JSON_OPTIONAL(j, descriptionFalse, std::string)) {}

    std::shared_ptr<NodeType> NodeJsonBoolean::getNodeType() const {
        return NodeType::JSON_BOOLEAN;
    }

    void NodeJsonBoolean::toJson(nlohmann::json &j) const {
        NodeBase::toJson(j);
        TO_JSON_OPTIONAL(j, descriptionTrue)
        TO_JSON_OPTIONAL(j, descriptionFalse)
    }

    ASTNode NodeJsonBoolean::getASTNode(TokenReader &tokenReader, const CPack &cpack) const {
        return tokenReader.readStringASTNode(this);
    }

} // CHelper::Node