//
// Created by Yancey on 2024/2/28.
//

#include "NodeJsonString.h"
#include "../../util/TokenUtil.h"

namespace CHelper::Node {

    NodeJsonString::NodeJsonString(const std::optional<std::string> &id,
                                   const std::optional<std::string> &description)
            : NodeBase(id, description, false) {}

    NodeJsonString::NodeJsonString(const nlohmann::json &j,
                                   [[maybe_unused]] const CPack &cpack)
            : NodeBase(j, false) {}

    std::shared_ptr<NodeType> NodeJsonString::getNodeType() const {
        return NodeType::JSON_STRING;
    }

    ASTNode NodeJsonString::getASTNode(TokenReader &tokenReader) const {
        tokenReader.push();
        ASTNode result = tokenReader.readStringASTNode(this);
        tokenReader.pop();
        std::string str = TokenUtil::toString(result.tokens);
        if (str.empty()) {
            result = ASTNode::simpleNode(this, result.tokens, ErrorReason::incomplete(
                    result.tokens, "字符串参数内容为空"));
        }else if (str[0] != '"') {
            result = ASTNode::simpleNode(this, result.tokens, ErrorReason::contentError(
                    result.tokens, "字符串参数内容应该在双引号内 -> str"));
        }
        return result;
    }

} // CHelper::Node