//
// Created by Yancey on 2023/12/10.
//

#include "NodeString.h"
#include "../../util/TokenUtil.h"

namespace CHelper::Node {

    static std::shared_ptr<NormalId> doubleQuoteMask = NormalId::make("\"", "双引号");

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
          canContainSpace(JsonUtil::read<bool>(j, "canContainSpace")),
          ignoreLater(JsonUtil::read<bool>(j, "ignoreLater")) {}

    NodeString::NodeString(BinaryReader &binaryReader,
                           [[maybe_unused]] const CPack &cpack)
        : NodeBase(binaryReader) {
        allowMissingString = false;
        canContainSpace = binaryReader.read<bool>();
        ignoreLater = binaryReader.read<bool>();
    }

    NodeType *NodeString::getNodeType() const {
        return NodeType::STRING.get();
    }

    void NodeString::toJson(nlohmann::json &j) const {
        NodeBase::toJson(j);
        JsonUtil::encode(j, "canContainSpace", canContainSpace);
        JsonUtil::encode(j, "ignoreLater", ignoreLater);
    }

    void NodeString::writeBinToFile(BinaryWriter &binaryWriter) const {
        NodeBase::writeBinToFile(binaryWriter);
        binaryWriter.encode(canContainSpace);
        binaryWriter.encode(ignoreLater);
    }

    ASTNode NodeString::getASTNode(TokenReader &tokenReader, const CPack *cpack) const {
        if (HEDLEY_UNLIKELY(ignoreLater)) {
            //后面的所有内容都算作这个字符串
            tokenReader.push();
            tokenReader.skipToLF();
            VectorView<Token> tokens = tokenReader.collect();
            if (!allowMissingString && TokenUtil::toString(tokens).empty()) {
                return ASTNode::simpleNode(this, tokens, ErrorReason::incomplete(tokens, "字符串参数内容为空"));
            } else {
                return ASTNode::simpleNode(this, tokens);
            }
        }
        tokenReader.push();
        ASTNode result = tokenReader.readStringASTNode(this);
        if (HEDLEY_UNLIKELY(allowMissingString && result.isError())) {
            tokenReader.restore();
            tokenReader.push();
            return ASTNode::simpleNode(this, tokenReader.collect());
        }
        tokenReader.pop();
        if (HEDLEY_UNLIKELY(!allowMissingString && result.tokens.isEmpty())) {
            return ASTNode::simpleNode(this, result.tokens, ErrorReason::incomplete(result.tokens, "字符串参数内容为空"));
        }
        if (HEDLEY_UNLIKELY(!canContainSpace)) {
            if (HEDLEY_UNLIKELY(TokenUtil::toString(result.tokens).find(' ') != std::string::npos)) {
                return ASTNode::simpleNode(this, result.tokens, ErrorReason::contentError(result.tokens, "字符串参数内容不可以包含空格"));
            }
            return result;
        }
        std::string str = TokenUtil::toString(result.tokens);
        if (HEDLEY_LIKELY((str.empty() || str[0] != '"'))) {
            return result;
        }
        auto convertResult = JsonUtil::jsonString2String(str);
        size_t offset = TokenUtil::getStartIndex(result.tokens);
        if (HEDLEY_UNLIKELY(convertResult.errorReason != nullptr)) {
            convertResult.errorReason->start += offset;
            convertResult.errorReason->end += offset;
            return ASTNode::simpleNode(this, result.tokens, convertResult.errorReason);
        }
        if (HEDLEY_UNLIKELY(!convertResult.isComplete)) {
            return ASTNode::simpleNode(this, result.tokens, ErrorReason::contentError(result.tokens, "字符串参数内容双引号不封闭 -> " + str));
        }
        return result;
    }

    bool NodeString::collectSuggestions(const ASTNode *astNode,
                                        size_t index,
                                        std::vector<Suggestions> &suggestions) const {
        if (HEDLEY_UNLIKELY(ignoreLater || !canContainSpace)) {
            return true;
        }
        std::string str = TokenUtil::toString(astNode->tokens)
                                  .substr(0, index - TokenUtil::getStartIndex(astNode->tokens));
        if (HEDLEY_UNLIKELY(str.empty())) {
            suggestions.push_back(Suggestions::singleSuggestion({index, index, doubleQuoteMask}));
            return true;
        }
        if (HEDLEY_LIKELY(str[0] != '"')) {
            return true;
        }
        auto convertResult = JsonUtil::jsonString2String(str);
        if (HEDLEY_LIKELY(!convertResult.isComplete)) {
            suggestions.push_back(Suggestions::singleSuggestion({index, index, doubleQuoteMask}));
        }
        return true;
    }

    void NodeString::collectStructure(const ASTNode *astNode,
                                      StructureBuilder &structure,
                                      bool isMustHave) const {
        structure.append(isMustHave, description.value_or("字符串"));
    }

}// namespace CHelper::Node