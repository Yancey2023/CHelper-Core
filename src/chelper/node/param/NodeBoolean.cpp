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
          descriptionTrue(JsonUtil::read<std::string>(j, "descriptionTrue")),
          descriptionFalse(JsonUtil::read<std::string>(j, "descriptionFalse")) {}

    NodeBoolean::NodeBoolean(BinaryReader &binaryReader,
                             [[maybe_unused]] const CPack &cpack)
        : NodeBase(binaryReader) {
        descriptionTrue = binaryReader.read<std::optional<std::string>>();
        descriptionFalse = binaryReader.read<std::optional<std::string>>();
    }

    NodeType *NodeBoolean::getNodeType() const {
        return NodeType::BOOLEAN.get();
    }

    void NodeBoolean::toJson(nlohmann::json &j) const {
        NodeBase::toJson(j);
        JsonUtil::encode(j, "descriptionTrue", descriptionTrue);
        JsonUtil::encode(j, "descriptionFalse", descriptionFalse);
    }

    void NodeBoolean::writeBinToFile(BinaryWriter &binaryWriter) const {
        NodeBase::writeBinToFile(binaryWriter);
        binaryWriter.encode(descriptionTrue);
        binaryWriter.encode(descriptionFalse);
    }

    ASTNode NodeBoolean::getASTNode(TokenReader &tokenReader, const CPack *cpack) const {
        ASTNode astNode = tokenReader.readStringASTNode(this);
        std::string str = TokenUtil::toString(astNode.tokens);
        if (HEDLEY_LIKELY(str == "true" || str == "false")) {
            return astNode;
        }
        VectorView<Token> tokens = astNode.tokens;
        return ASTNode::andNode(this, {std::move(astNode)}, tokens, ErrorReason::contentError(tokens, "内容不匹配，应该为布尔值，但当前内容为" + str));
    }

    bool NodeBoolean::collectSuggestions(const ASTNode *astNode,
                                         size_t index,
                                         std::vector<Suggestions> &suggestions) const {
        std::string str = TokenUtil::toString(astNode->tokens)
                                  .substr(0, index - TokenUtil::getStartIndex(astNode->tokens));
        Suggestions suggestions1;
        if (HEDLEY_UNLIKELY(std::string("true").find(str) != std::string::npos)) {
            std::shared_ptr<NormalId> id;
            id->name = "true";
            id->description = descriptionTrue;
            suggestions1.suggestions.emplace_back(astNode->tokens, std::move(id));
        }
        if (HEDLEY_UNLIKELY(std::string("false").find(str) != std::string::npos)) {
            std::shared_ptr<NormalId> id;
            id->name = "false";
            id->description = descriptionFalse;
            suggestions1.suggestions.emplace_back(astNode->tokens, std::move(id));
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

}// namespace CHelper::Node