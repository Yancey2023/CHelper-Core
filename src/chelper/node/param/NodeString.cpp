//
// Created by Yancey on 2023/12/10.
//

#include "NodeString.h"
#include "../util/NodeSingleSymbol.h"
#include "../../util/TokenUtil.h"

namespace CHelper::Node {

    NodeString::NodeString(const std::optional<std::string> &id,
                           const std::optional<std::string> &description,
                           bool allowMissingString,
                           const bool canContainSpace,
                           const bool ignoreLater)
            : NodeBase(id, description, false),
              allowMissingString(allowMissingString),
              canContainSpace(canContainSpace),
              ignoreLater(ignoreLater) {}

    NodeString::NodeString(const nlohmann::json &j,
                           [[maybe_unused]] const CPack &cpack)
            : NodeBase(j, true),
              allowMissingString(false),
              canContainSpace(FROM_JSON(j, canContainSpace, bool)),
              ignoreLater(FROM_JSON(j, ignoreLater, bool)) {}

    std::shared_ptr<NodeType> NodeString::getNodeType() const {
        return NodeType::STRING;
    }

    void NodeString::toJson(nlohmann::json &j) const {
        NodeBase::toJson(j);
        TO_JSON(j, canContainSpace);
        TO_JSON(j, ignoreLater);
    }

    ASTNode NodeString::getASTNode(TokenReader &tokenReader, const CPack &cpack) const {
        if (ignoreLater) {
            //后面的所有内容都算作这个字符串
            tokenReader.push();
            tokenReader.skipToLF();
            return ASTNode::simpleNode(this, tokenReader.collect());
        }
        tokenReader.push();
        ASTNode result = tokenReader.readStringASTNode(this);
        if (result.isError() && allowMissingString) {
            tokenReader.restore();
            tokenReader.push();
            return ASTNode::simpleNode(this, tokenReader.collect());
        }
        tokenReader.pop();
        if (!allowMissingString && result.tokens.isEmpty()) {
            result = ASTNode::simpleNode(this, result.tokens, ErrorReason::incomplete(
                    result.tokens, "字符串参数内容为空"));
        } else if (!canContainSpace && TokenUtil::toString(result.tokens).find(' ') != std::string::npos) {
            result = ASTNode::simpleNode(this, result.tokens, ErrorReason::contentError(
                    result.tokens, "字符串参数内容不可以包含空格"));
        }
        return result;
    }

    void NodeString::collectStructure(const ASTNode *astNode,
                                      StructureBuilder &structure,
                                      bool isMustHave) const {
        if (astNode == nullptr || astNode->id == "string") {
            structure.append(isMustHave, description.value_or("字符串"));
        }
    }

} // CHelper::Node