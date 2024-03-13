//
// Created by Yancey on 2024/2/28.
//

#include "NodeJsonBoolean.h"
#include "../../util/TokenUtil.h"

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
              descriptionTrue(JsonUtil::fromJsonOptional<std::string>(j, "descriptionTrue")),
              descriptionFalse(JsonUtil::fromJsonOptional<std::string>(j, "descriptionFalse")) {}

    NodeType *NodeJsonBoolean::getNodeType() const {
        return NodeType::JSON_BOOLEAN.get();
    }

    void NodeJsonBoolean::toJson(nlohmann::json &j) const {
        NodeBase::toJson(j);
        JsonUtil::toJsonOptional(j, "descriptionTrue", descriptionTrue);
        JsonUtil::toJsonOptional(j, "descriptionFalse", descriptionFalse);
    }

    ASTNode NodeJsonBoolean::getASTNode(TokenReader &tokenReader, const CPack *cpack) const {
        ASTNode astNode = tokenReader.readStringASTNode(this);
        std::string str = TokenUtil::toString(astNode.tokens);
        if (str == "true" || str == "false") {
            return astNode;
        }
        VectorView <Token> tokens = astNode.tokens;
        return ASTNode::andNode(this, {std::move(astNode)}, tokens);
    }

} // CHelper::Node