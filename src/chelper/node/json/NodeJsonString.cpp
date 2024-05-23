//
// Created by Yancey on 2024/2/28.
//

#include "NodeJsonString.h"
#include "../../parser/Parser.h"
#include "../../util/TokenUtil.h"

namespace CHelper::Node {

    static std::shared_ptr<NormalId> doubleQuoteMask = NormalId::make("\"", "双引号");

    static std::vector<std::unique_ptr<NodeBase>>
    getDataFromJson(const nlohmann::json &j, const CPack &cpack) {
        std::vector<std::unique_ptr<NodeBase>> result;
        const auto &it = j.find("data");
        if (HEDLEY_LIKELY(it != j.end())) {
            result.reserve(it->size());
            for (const auto &item: it.value()) {
                result.push_back(NodeBase::getNodeFromJson(item, cpack));
            }
        }
        return std::move(result);
    }

    static std::vector<std::unique_ptr<NodeBase>>
    readDataFromBinary(BinaryReader &binaryReader, const CPack &cpack) {
        size_t size = binaryReader.readSize();
        std::vector<std::unique_ptr<NodeBase>> data;
        data.reserve(size);
        for (int i = 0; i < size; ++i) {
            data.push_back(NodeBase::getNodeFromBinary(binaryReader, cpack));
        }
        return std::move(data);
    }

    static std::unique_ptr<NodeBase>
    getNodeDataFromData(const std::vector<std::unique_ptr<NodeBase>> &data) {
        std::vector<const NodeBase *> nodeDataElement;
        nodeDataElement.reserve(data.size());
        for (const auto &item: data) {
            nodeDataElement.push_back(item.get());
        }
        return std::make_unique<NodeOr>("JSON_STRING_DATA", "JSON字符串内容",
                                        std::move(nodeDataElement), false);
    }

    NodeJsonString::NodeJsonString(const std::optional<std::string> &id,
                                   const std::optional<std::string> &description,
                                   std::vector<std::unique_ptr<NodeBase>> data)
        : NodeBase(id, description, false),
          data(std::move(data)),
          nodeData(getNodeDataFromData(this->data)) {}

    NodeJsonString::NodeJsonString(const nlohmann::json &j,
                                   const CPack &cpack)
        : NodeBase(j, false),
          data(getDataFromJson(j, cpack)),
          nodeData(getNodeDataFromData(data)) {}

    NodeJsonString::NodeJsonString(BinaryReader &binaryReader,
                                   const CPack &cpack)
        : NodeBase(binaryReader),
          data(readDataFromBinary(binaryReader, cpack)),
          nodeData(getNodeDataFromData(data)) {}

    NodeType *NodeJsonString::getNodeType() const {
        return NodeType::JSON_STRING.get();
    }

    void NodeJsonString::toJson(nlohmann::json &j) const {
        NodeBase::toJson(j);
        JsonUtil::encode(j, "data", data);
    }

    void NodeJsonString::writeBinToFile(BinaryWriter &binaryWriter) const {
        NodeBase::writeBinToFile(binaryWriter);
        binaryWriter.encode(data);
    }

    static std::pair<ASTNode, JsonUtil::ConvertResult>
    getInnerASTNode(const NodeJsonString *node,
                    const VectorView<Token> &tokens,
                    const std::string &content,
                    const CPack *cpack,
                    const NodeBase *mainNode) {
        auto convertResult = JsonUtil::jsonString2String(content);
        if (HEDLEY_UNLIKELY(convertResult.errorReason != nullptr)) {
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
            return ASTNode::simpleNode(this, result.tokens, ErrorReason::incomplete(result.tokens, "字符串参数内容为空"));
        } else if (HEDLEY_UNLIKELY(str[0] != '"')) {
            return ASTNode::simpleNode(this, result.tokens, ErrorReason::contentError(result.tokens, "字符串参数内容应该在双引号内 -> " + str));
        }
        VectorView<Token> tokens = result.tokens;
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
        if (HEDLEY_UNLIKELY(errorReason == nullptr)) {
            for (auto &item: newResult.errorReasons) {
                item = std::make_shared<ErrorReason>(
                        item->level,
                        innerNode.second.convert(item->start) + offset,
                        innerNode.second.convert(item->end) + offset,
                        item->errorReason);
            }
        }
        return std::move(newResult);
    }

    bool NodeJsonString::collectIdError(const ASTNode *astNode,
                                        std::vector<std::shared_ptr<ErrorReason>> &idErrorReasons) const {
        if (HEDLEY_UNLIKELY(astNode->id == "inner")) {
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
        if (HEDLEY_UNLIKELY(str.empty())) {
            suggestions.push_back(Suggestions::singleSuggestion({index, index, doubleQuoteMask}));
            return true;
        }
        auto convertResult = JsonUtil::jsonString2String(str);
        if (HEDLEY_UNLIKELY(astNode->id == "inner")) {
            size_t offset = TokenUtil::getStartIndex(astNode->tokens) + 1;
            Suggestions suggestions1;
            suggestions1.suggestions = astNode->childNodes[0].getSuggestions(index - offset);
            for (auto &item: suggestions1.suggestions) {
                item.start = convertResult.convert(item.start) + offset;
                item.end = convertResult.convert(item.end) + offset;
                std::string convertStr = JsonUtil::string2jsonString(item.content->name);
                if (HEDLEY_UNLIKELY(convertStr.size() != str.size())) {
                    item.content = NormalId::make(convertStr, item.content->description);
                }
            }
            if (HEDLEY_LIKELY(astNode->hasChildNode() && !astNode->childNodes[0].isError() &&
                              convertResult.errorReason == nullptr && !convertResult.isComplete)) {
                suggestions1.suggestions.emplace_back(index, index, doubleQuoteMask);
            }
            suggestions1.markFiltered();
            suggestions.push_back(std::move(suggestions1));
        } else if (HEDLEY_LIKELY(convertResult.errorReason == nullptr && !convertResult.isComplete)) {
            suggestions.push_back(Suggestions::singleSuggestion({index, index, doubleQuoteMask}));
        }
        return true;
    }

}// namespace CHelper::Node