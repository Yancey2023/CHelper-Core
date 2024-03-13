//
// Created by Yancey on 2024/2/28.
//

#include "NodeJsonNull.h"
#include "../../util/TokenUtil.h"

namespace CHelper::Node {

    NodeJsonNull::NodeJsonNull(const std::optional<std::string> &id,
                               const std::optional<std::string> &description)
            : NodeBase(id, description, false) {}

    NodeJsonNull::NodeJsonNull(const nlohmann::json &j,
                               [[maybe_unused]]const CPack &cpack)
            : NodeBase(j, false) {}

    NodeType *NodeJsonNull::getNodeType() const {
        return NodeType::JSON_NULL.get();
    }

    ASTNode NodeJsonNull::getASTNode(TokenReader &tokenReader, const CPack *cpack) const {
        tokenReader.push();
        auto result = tokenReader.readStringASTNode(this);
        tokenReader.pop();
        std::string str = TokenUtil::toString(result.tokens);
        if (str.empty()) {
            VectorView <Token> tokens = result.tokens;
            return ASTNode::andNode(this, {std::move(result)}, tokens, ErrorReason::contentError(
                    tokens, "null参数为空"));
        } else if (str != "null") {
            VectorView <Token> tokens = result.tokens;
            return ASTNode::andNode(this, {std::move(result)}, tokens, ErrorReason::contentError(
                    tokens, "内容不是null -> " + str));
        }
        return result;
    }

    bool NodeJsonNull::collectSuggestions(const ASTNode *astNode,
                                          size_t index,
                                          std::vector<Suggestions> &suggestions) const {
        std::string str = TokenUtil::toString(astNode->tokens)
                .substr(0, index - TokenUtil::getStartIndex(astNode->tokens));
        if (StringUtil::isStartOf("null", str)) {
            suggestions.push_back(Suggestions::singleSuggestion(
                    {astNode->tokens, std::make_shared<NormalId>("null", "null参数")}));
        }
        return true;
    }

} // CHelper::Node