//
// Created by Yancey on 2023/12/10.
//

#include "NodeString.h"
#include "../util/NodeSingleSymbol.h"
#include "../../util/TokenUtil.h"

namespace CHelper::Node {

    static std::shared_ptr<NormalId> doubleQuoteMask = std::make_shared<NormalId>("\"", "双引号");

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
              canContainSpace(JsonUtil::fromJson<bool>(j, "canContainSpace")),
              ignoreLater(JsonUtil::fromJson<bool>(j, "ignoreLater")) {}

    NodeType *NodeString::getNodeType() const {
        return NodeType::STRING.get();
    }

    void NodeString::toJson(nlohmann::json &j) const {
        NodeBase::toJson(j);
        JsonUtil::toJson(j, "canContainSpace", canContainSpace);
        JsonUtil::toJson(j, "ignoreLater", ignoreLater);
    }

    ASTNode NodeString::getASTNode(TokenReader &tokenReader, const CPack *cpack) const {
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
            return ASTNode::simpleNode(this, result.tokens, ErrorReason::incomplete(
                    result.tokens, "字符串参数内容为空"));
        }
        if (!canContainSpace) {
            if (TokenUtil::toString(result.tokens).find(' ') != std::string::npos) {
                return ASTNode::simpleNode(this, result.tokens, ErrorReason::contentError(
                        result.tokens, "字符串参数内容不可以包含空格"));
            }
            return result;
        }
        std::string str = TokenUtil::toString(result.tokens);
        if (str.empty() || str[0] != '"') {
            return result;
        }
        auto convertResult = JsonUtil::jsonString2String(str);
        size_t offset = TokenUtil::getStartIndex(result.tokens);
        if (convertResult.errorReason != nullptr) {
            convertResult.errorReason->start += offset;
            convertResult.errorReason->end += offset;
            return ASTNode::simpleNode(this, result.tokens, convertResult.errorReason);
        }
        return result;
    }

    bool NodeString::collectSuggestions(const ASTNode *astNode,
                                        size_t index,
                                        std::vector<Suggestions> &suggestions) const {
        if (ignoreLater || !canContainSpace) {
            return true;
        }
        std::string str = TokenUtil::toString(astNode->tokens)
                .substr(0, index - TokenUtil::getStartIndex(astNode->tokens));
        if (str.empty()) {
            suggestions.push_back(Suggestions::singleSuggestion({index, index, doubleQuoteMask}));
            return true;
        }
        if (str[0] != '"') {
            return true;
        }
        auto convertResult = JsonUtil::jsonString2String(str);
        if (!convertResult.isComplete) {
            suggestions.push_back(Suggestions::singleSuggestion({index, index, doubleQuoteMask}));
        }
        return true;
    }

    void NodeString::collectStructure(const ASTNode *astNode,
                                      StructureBuilder &structure,
                                      bool isMustHave) const {
        structure.append(isMustHave, description.value_or("字符串"));
    }

} // CHelper::Node