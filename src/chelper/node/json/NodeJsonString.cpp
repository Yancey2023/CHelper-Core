//
// Created by Yancey on 2024/2/28.
//

#include "NodeJsonString.h"
#include "../../util/TokenUtil.h"
#include "../../parser/Parser.h"

namespace CHelper::Node {

    static std::shared_ptr<NormalId> doubleQuoteMask = std::make_shared<NormalId>("\"", "双引号");

    static std::vector<std::unique_ptr<NodeBase>> getDataFromCpack(const nlohmann::json &j,
                                                                   const CPack &cpack) {
        std::vector<std::unique_ptr<NodeBase>> result;
        if (j.contains("data")) {
            for (const auto &item: j.at("data")) {
                result.push_back(NodeJsonString::getNodeFromJson(item, cpack));
            }
        }
        return result;
    }

    static std::unique_ptr<NodeBase>
    getNodeDataFromData(const std::vector<std::unique_ptr<NodeBase>> &data) {
        std::vector<const NodeBase *> nodeDataElement;
        nodeDataElement.reserve(data.size());
        for (const auto &item: data) {
            nodeDataElement.push_back(item.get());
        }
        return std::make_unique<NodeOr>("JSON_STRING_DATA", "JSON字符串内容",
                                        nodeDataElement, false);
    }

    NodeJsonString::NodeJsonString(const std::optional<std::string> &id,
                                   const std::optional<std::string> &description,
                                   std::vector<std::unique_ptr<NodeBase>> data)
            : NodeBase(id, description, false),
              data(std::move(data)),
              nodeData(getNodeDataFromData(data)) {}

    NodeJsonString::NodeJsonString(const nlohmann::json &j,
                                   const CPack &cpack)
            : NodeBase(j, false),
              data(getDataFromCpack(j, cpack)),
              nodeData(getNodeDataFromData(data)) {}

    NodeType *NodeJsonString::getNodeType() const {
        return NodeType::JSON_STRING.get();
    }

    void NodeJsonString::toJson(nlohmann::json &j) const {
        NodeBase::toJson(j);
        JsonUtil::toJson(j, "data", data);
    }

    static std::pair<ASTNode, JsonUtil::ConvertResult> getInnerASTNode(const NodeJsonString *node,
                                                                       const VectorView <Token> &tokens,
                                                                       const std::string &content,
                                                                       const CPack *cpack,
                                                                       const NodeBase *mainNode) {
        auto convertResult = JsonUtil::jsonString2String(content);
        if (convertResult.errorReason != nullptr) {
            convertResult.errorReason->start--;
            convertResult.errorReason->end--;
            return {ASTNode::simpleNode(node, tokens, convertResult.errorReason), std::move(convertResult)};
        }
        auto tokenReader = TokenReader(std::make_shared<std::vector<Token>>(
                Lexer::lex(StringReader(convertResult.result, "unknown"))));
        Profile::push("start parsing: " + content);
        DEBUG_GET_NODE_BEGIN(mainNode)
        auto result = mainNode->getASTNode(tokenReader, cpack);
        DEBUG_GET_NODE_END(mainNode)
        Profile::pop();
        return {std::move(result), std::move(convertResult)};
    }

    ASTNode NodeJsonString::getASTNode(TokenReader &tokenReader, const CPack *cpack) const {
        tokenReader.push();
        ASTNode result = tokenReader.readStringASTNode(this);
        tokenReader.pop();
        std::string str = TokenUtil::toString(result.tokens);
        if (HEDLEY_UNLIKELY(str.empty())) {
            return ASTNode::simpleNode(this, result.tokens, ErrorReason::incomplete(
                    result.tokens, "字符串参数内容为空"));
        } else if (HEDLEY_UNLIKELY(str[0] != '"')) {
            return ASTNode::simpleNode(this, result.tokens, ErrorReason::contentError(
                    result.tokens, "字符串参数内容应该在双引号内 -> " + str));
        }
        VectorView <Token> tokens = result.tokens;
        std::shared_ptr<ErrorReason> errorReason;
        if (HEDLEY_LIKELY(str.size() <= 1 || str[str.size() - 1] != '"')) {
            errorReason = ErrorReason::contentError(tokens, "字符串参数内容应该在双引号内 -> " + str);
        }
        if (HEDLEY_LIKELY(data.empty())) {
            return ASTNode::andNode(this, {std::move(result)}, tokens, errorReason);
        }
        size_t offset = TokenUtil::getStartIndex(tokens) + 1;
        auto innerNode = getInnerASTNode(this, tokens, str, cpack, nodeData.get());
        ASTNode newResult = ASTNode::andNode(this, {std::move(innerNode.first)}, tokens, errorReason, "inner");
        if (errorReason == nullptr) {
            size_t size = newResult.errorReasons.size();
            for (int i = 0; i < size; ++i) {
                auto item = newResult.errorReasons[i];
                newResult.errorReasons[i] = std::make_shared<ErrorReason>(
                        item->level,
                        innerNode.second.convert(item->start) + offset,
                        innerNode.second.convert(item->end) + offset,
                        item->errorReason
                );
            }
        }
        return newResult;
    }

    bool NodeJsonString::collectIdError(const ASTNode *astNode,
                                        std::vector<std::shared_ptr<ErrorReason>> &idErrorReasons) const {
        if (HEDLEY_LIKELY(astNode->id == "inner")) {
            auto convertResult = JsonUtil::jsonString2String(TokenUtil::toString(astNode->tokens));
            size_t offset = TokenUtil::getStartIndex(astNode->tokens) + 1;
            for (const auto &item: astNode->childNodes[0].getIdErrors()) {
                item->start = convertResult.convert(item->start) + offset;
                item->end = convertResult.convert(item->end) + offset;
                idErrorReasons.push_back(item);
            }
        }
        return true;
    }

    bool NodeJsonString::collectSuggestions(const ASTNode *astNode,
                                            size_t index,
                                            std::vector<Suggestions> &suggestions) const {
        std::string str = TokenUtil::toString(astNode->tokens)
                .substr(0, index - TokenUtil::getStartIndex(astNode->tokens));
        if (str.empty()) {
            suggestions.push_back(Suggestions::singleSuggestion({index, index, doubleQuoteMask}));
            return true;
        }
        if (astNode->id == "inner") {
            Suggestions suggestions1;
            auto convertResult = JsonUtil::jsonString2String(str);
            size_t offset = TokenUtil::getStartIndex(astNode->tokens) + 1;
            for (auto &item: astNode->childNodes[0].getSuggestions(index - offset)) {
                item.start = convertResult.convert(item.start) + offset;
                item.end = convertResult.convert(item.end) + offset;
                item.content = std::make_shared<NormalId>(
                        JsonUtil::string2jsonString(item.content->name), item.content->description);
                suggestions1.suggestions.push_back(item);
            }
            if (astNode->hasChildNode() && !astNode->childNodes[0].isError()) {
                if (str.length() == 1 || str[str.length() - 1] != '"' ||
                    (convertResult.errorReason == nullptr && !convertResult.isComplete)) {
                    suggestions1.suggestions.emplace_back(index, index, doubleQuoteMask);
                }
            }
            suggestions1.markFiltered();
            suggestions.push_back(std::move(suggestions1));
            return true;
        } else {
            if (str.length() == 1 || str[str.length() - 1] != '"') {
                suggestions.push_back(Suggestions::singleSuggestion({index, index, doubleQuoteMask}));
                return true;
            }
            auto convertResult = JsonUtil::jsonString2String(str);
            if (!convertResult.isComplete) {
                suggestions.push_back(Suggestions::singleSuggestion({index, index, doubleQuoteMask}));
            }
            return true;
        }
    }

} // CHelper::Node