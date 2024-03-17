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
              descriptionTrue(JsonUtil::fromJsonOptionalLikely<std::string>(j, "descriptionTrue")),
              descriptionFalse(JsonUtil::fromJsonOptionalLikely<std::string>(j, "descriptionFalse")) {}

    NodeType *NodeBoolean::getNodeType() const {
        return NodeType::BOOLEAN.get();
    }

    void NodeBoolean::toJson(nlohmann::json &j) const {
        NodeBase::toJson(j);
        JsonUtil::toJsonOptionalLikely(j, "descriptionTrue", descriptionTrue);
        JsonUtil::toJsonOptionalLikely(j, "descriptionFalse", descriptionFalse);
    }

    ASTNode NodeBoolean::getASTNode(TokenReader &tokenReader, const CPack *cpack) const {
        ASTNode astNode = tokenReader.readStringASTNode(this);
        std::string str = TokenUtil::toString(astNode.tokens);
        if (str == "true" || str == "false") {
            return astNode;
        }
        VectorView <Token> tokens = astNode.tokens;
        return ASTNode::andNode(this, {std::move(astNode)}, tokens, ErrorReason::contentError(
                tokens, "内容不匹配，应该为布尔值，但当前内容为" + str));
    }

    bool NodeBoolean::collectSuggestions(const ASTNode *astNode,
                                         size_t index,
                                         std::vector<Suggestions> &suggestions) const {
        std::string str = TokenUtil::toString(astNode->tokens)
                .substr(0, index - TokenUtil::getStartIndex(astNode->tokens));
        Suggestions suggestions1;
        if (str.find("true") != std::string::npos) {
            suggestions1.suggestions.emplace_back(astNode->tokens, std::make_shared<NormalId>(
                    "true", descriptionTrue));
        }
        if (str.find("false") != std::string::npos) {
            suggestions1.suggestions.emplace_back(astNode->tokens, std::make_shared<NormalId>(
                    "false", descriptionFalse));
        }
        suggestions1.markFiltered();
        suggestions.push_back(std::move(suggestions1));
        return true;
    }

    void NodeBoolean::collectStructure(const ASTNode *astNode,
                                       StructureBuilder &structure,
                                       bool isMustHave) const {
        structure.append(isMustHave, description.value_or("布尔值"));
    }

} // CHelper::Node