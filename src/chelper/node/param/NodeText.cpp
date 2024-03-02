//
// Created by Yancey on 2023/12/2.
//

#include "NodeText.h"
#include "../../util/TokenUtil.h"

namespace CHelper::Node {

    NodeText::NodeText(const std::optional<std::string> &id,
                                      const std::optional<std::string> &description,
                                      const std::shared_ptr<NormalId> &data,
                                      ASTNode(*getTextASTNode)(const NodeBase *node, TokenReader &tokenReader))
            : NodeBase(id, description, false),
              data(data),
              getTextASTNode(getTextASTNode) {}

    NodeText::NodeText(const nlohmann::json &j,
                                      [[maybe_unused]] const CPack &cpack)
            : NodeBase(j, true),
              data(std::make_shared<NormalId>(j.at("data"))),
              getTextASTNode([](const NodeBase *node, TokenReader &tokenReader) -> ASTNode {
                  return tokenReader.readStringASTNode(node);
              }) {}

    std::shared_ptr<NodeType> NodeText::getNodeType() const {
        return NodeType::TEXT;
    }

    void NodeText::toJson(nlohmann::json &j) const {
        NodeBase::toJson(j);
        nlohmann::json dataJson;
        data->toJson(dataJson);
        j.push_back(dataJson);
    }

    ASTNode NodeText::getASTNode(TokenReader &tokenReader, const CPack &cpack) const {
        DEBUG_GET_NODE_BEGIN(this)
        auto result = getTextASTNode(this, tokenReader);
        DEBUG_GET_NODE_END(this)
        std::string str = TokenUtil::toString(result.tokens);
        if (str != data->name) {
            if (str.empty()) {
                return ASTNode::andNode(this, {result}, result.tokens, ErrorReason::contentError(
                        result.tokens, "找不到内容 -> " + data->name));
            } else {
                return ASTNode::andNode(this, {result}, result.tokens, ErrorReason::contentError(
                        result.tokens, "找不到含义 -> " + str));
            }
        }
        return result;
    }

    bool NodeText::collectSuggestions(const ASTNode *astNode,
                                      size_t index,
                                      std::vector<Suggestion> &suggestions) const {
        std::string str = TokenUtil::toString(astNode->tokens)
                .substr(0, index - TokenUtil::getStartIndex(astNode->tokens));
        if (StringUtil::isStartOf(data->name, str)) {
            suggestions.emplace_back(astNode->tokens, data);
        }
        return true;
    }

    void NodeText::collectStructure(const ASTNode *astNode,
                                    StructureBuilder &structure,
                                    bool isMustHave) const {
        structure.appendWhiteSpace().append(data->name);
    }

} // CHelper::Node