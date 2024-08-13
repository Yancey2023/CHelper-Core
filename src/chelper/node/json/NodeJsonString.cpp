//
// Created by Yancey on 2024/2/28.
//

#include "NodeJsonString.h"
#include "../../parser/Parser.h"

namespace CHelper::Node {

    static std::shared_ptr<NormalId> doubleQuoteMask = NormalId::make("\"", "双引号");

    NodeJsonString::NodeJsonString(const std::optional<std::string> &id,
                                   const std::optional<std::string> &description)
        : NodeBase(id, description, false) {
        nodeData = std::make_unique<NodeOr>(
                "JSON_STRING_DATA", "JSON字符串内容",
                std::vector<const NodeBase *>(), false);
    }

    void NodeJsonString::init(const CPack &cpack) {
        if (HEDLEY_UNLIKELY(data.has_value())) {
            for (const auto &item: data.value()) {
                item->init(cpack);
            }
        }
        std::vector<const NodeBase *> nodeDataElement;
        if (HEDLEY_UNLIKELY(data.has_value())) {
            nodeDataElement.reserve(data.value().size());
            for (const auto &item: data.value()) {
                nodeDataElement.push_back(item.get());
            }
        }
        nodeData = std::make_unique<NodeOr>(
                "JSON_STRING_DATA", "JSON字符串内容",
                std::move(nodeDataElement), false);
    }

    NodeType *NodeJsonString::getNodeType() const {
        return NodeType::JSON_STRING.get();
    }

    static std::pair<ASTNode, JsonUtil::ConvertResult>
    getInnerASTNode(const NodeJsonString *node,
                    const TokensView &tokens,
                    const std::string &content,
                    const CPack *cpack,
                    const NodeBase *mainNode) {
        auto convertResult = JsonUtil::jsonString2String(content);
        if (HEDLEY_UNLIKELY(convertResult.errorReason != nullptr)) {
            convertResult.errorReason->start--;
            convertResult.errorReason->end--;
            return {ASTNode::simpleNode(node, tokens, convertResult.errorReason), std::move(convertResult)};
        }
        auto tokenReader = TokenReader(std::make_shared<LexerResult>(Lexer::lex(convertResult.result)));
#if CHelperTest == true
        Profile::push("start parsing: {}", content);
#endif
        DEBUG_GET_NODE_BEGIN(mainNode)
        ASTNode result = Parser::parse(convertResult.result, cpack, mainNode);
        DEBUG_GET_NODE_END(mainNode)
#if CHelperTest == true
        Profile::pop();
#endif
        return {std::move(result), std::move(convertResult)};
    }

    ASTNode NodeJsonString::getASTNode(TokenReader &tokenReader, const CPack *cpack) const {
        tokenReader.push();
        ASTNode result = tokenReader.readStringASTNode(this);
        tokenReader.pop();
        TokensView tokens = result.tokens;
        std::string_view str = tokens.toString();
        if (HEDLEY_UNLIKELY(str.empty())) {
            return ASTNode::simpleNode(this, tokens, ErrorReason::incomplete(tokens, "字符串参数内容为空"));
        } else if (HEDLEY_UNLIKELY(str[0] != '"')) {
            return ASTNode::simpleNode(this, tokens, ErrorReason::contentError(tokens, "字符串参数内容应该在双引号内 -> " + std::string(str)));
        }
        std::shared_ptr<ErrorReason> errorReason;
        if (HEDLEY_LIKELY(str.size() <= 1 || str[str.size() - 1] != '"')) {
            errorReason = ErrorReason::contentError(tokens, "字符串参数内容应该在双引号内 -> " + std::string(str));
        }
        if (HEDLEY_LIKELY(!data.has_value() || data->empty())) {
            return ASTNode::simpleNode(this, tokens, errorReason);
        }
        size_t offset = tokens.getStartIndex() + 1;
        auto innerNode = getInnerASTNode(this, tokens, std::string(str), cpack, nodeData.get());
        ASTNode newResult = ASTNode::andNode(this, {std::move(innerNode.first)}, tokens, errorReason, ASTNodeId::NODE_STRING_INNER);
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
        if (HEDLEY_UNLIKELY(astNode->id == ASTNodeId::NODE_STRING_INNER)) {
            auto convertResult = JsonUtil::jsonString2String(std::string(astNode->tokens.toString()));
            size_t offset = astNode->tokens.getStartIndex() + 1;
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
        std::string_view str = astNode->tokens.toString()
                                       .substr(0, index - astNode->tokens.getStartIndex());
        if (HEDLEY_UNLIKELY(str.empty())) {
            suggestions.push_back(Suggestions::singleSymbolSuggestion({index, index, false, doubleQuoteMask}));
            return true;
        }
        auto convertResult = JsonUtil::jsonString2String(std::string(str));
        if (HEDLEY_UNLIKELY(astNode->id == ASTNodeId::NODE_STRING_INNER)) {
            size_t offset = astNode->tokens.getStartIndex() + 1;
            Suggestions suggestions1(SuggestionsType::LITERAL);
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
                suggestions1.suggestions.emplace_back(index, index, false, doubleQuoteMask);
            }
            suggestions1.markFiltered();
            suggestions.push_back(std::move(suggestions1));
        } else if (HEDLEY_LIKELY(convertResult.errorReason == nullptr && !convertResult.isComplete)) {
            suggestions.push_back(Suggestions::singleSymbolSuggestion({index, index, false, doubleQuoteMask}));
        }
        return true;
    }

    bool NodeJsonString::collectColor(const ASTNode *astNode,
                                      ColoredString &coloredString,
                                      const Theme &theme) const {
        if (astNode->id != ASTNodeId::NODE_STRING_INNER) {
            coloredString.setColor(astNode->tokens, theme.colorString);
            return false;
        }
        coloredString.setColor(astNode->tokens.getStartIndex(), theme.colorString);
        std::string_view str = astNode->tokens.toString();
        auto convertResult = JsonUtil::jsonString2String(std::string(str));
        if (convertResult.isComplete) {
            coloredString.setColor(astNode->tokens.getEndIndex() - 1, theme.colorString);
        }
        ColoredString coloredString1 = astNode->childNodes[0].getColors(theme);
        int index = 0;
        for (int i = 0; i < convertResult.result.size(); ++i) {
            size_t end = convertResult.convert(i + 1);
            while (index < end) {
                coloredString.setColor(astNode->tokens.getStartIndex() + index + 1, coloredString1.colors[i]);
                index++;
            }
        }
        return true;
    }

    CODEC_NODE(NodeJsonString, data)

}// namespace CHelper::Node