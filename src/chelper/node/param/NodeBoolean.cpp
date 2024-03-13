//
// Created by Yancey on 2023/11/11.
//

#include "NodeBoolean.h"
#include "../../util/TokenUtil.h"

namespace CHelper::Node {

    NodeBoolean::NodeBoolean(const std::optional<std::string> &id,
                             const std::optional<std::string> &description,
                             const std::optional<std::string> &descriptionTrue,
                             const std::optional<std::string> &descriptionFalse)
            : NodeBase(id, description, false),
              descriptionTrue(descriptionTrue),
              descriptionFalse(descriptionFalse) {}

    NodeBoolean::NodeBoolean(const nlohmann::json &j,
                             [[maybe_unused]] const CPack &cpack)
            : NodeBase(j, true),
              descriptionTrue(JsonUtil::fromJsonOptional<std::string>(j, "descriptionTrue")),
              descriptionFalse(JsonUtil::fromJsonOptional<std::string>(j, "descriptionFalse")) {}

    NodeType *NodeBoolean::getNodeType() const {
        return NodeType::BOOLEAN.get();
    }

    void NodeBoolean::toJson(nlohmann::json &j) const {
        NodeBase::toJson(j);
        JsonUtil::toJsonOptional(j, "descriptionTrue", descriptionTrue);
        JsonUtil::toJsonOptional(j, "descriptionFalse", descriptionFalse);
    }

    ASTNode NodeBoolean::getASTNode(TokenReader &tokenReader, const CPack *cpack) const {
        ASTNode astNode = tokenReader.readStringASTNode(this);
        std::string str = TokenUtil::toString(astNode.tokens);
        if (str == "true" || str == "false") {
            return astNode;
        }
        VectorView <Token> tokens = astNode.tokens;
        return ASTNode::andNode(this, {std::move(astNode)}, tokens);
    }

    void NodeBoolean::collectStructure(const ASTNode *astNode,
                                       StructureBuilder &structure,
                                       bool isMustHave) const {
        structure.append(isMustHave, description.value_or("布尔值"));
    }

} // CHelper::Node