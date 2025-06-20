//
// Created by Yancey on 2024/2/28.
//

#include <chelper/lexer/Lexer.h>
#include <chelper/node/NodeType.h>
#include <chelper/node/json/NodeJsonString.h>
#include <chelper/node/util/NodeOr.h>
#include <chelper/parser/Parser.h>

namespace CHelper::Node {

    static std::shared_ptr<NormalId> doubleQuoteMask = NormalId::make(u"\"", u"双引号");

    NodeJsonString::NodeJsonString(const std::optional<std::string> &id,
                                   const std::optional<std::u16string> &description)
        : NodeSerializable(id, description, false) {
        nodeData = std::make_unique<NodeOr>(std::vector<const NodeBase *>(), false);
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
        nodeData = std::make_unique<NodeOr>(std::move(nodeDataElement), false);
    }

    NodeTypeId::NodeTypeId NodeJsonString::getNodeType() const {
        return NodeTypeId::JSON_STRING;
    }

    static std::pair<ASTNode, JsonUtil::ConvertResult>
    getInnerASTNode(const NodeJsonString *node,
                    const TokensView &tokens,
                    const std::u16string &content,
                    const CPack *cpack,
                    const NodeBase *mainNode) {
        auto convertResult = JsonUtil::jsonString2String(content);
        if (HEDLEY_UNLIKELY(convertResult.errorReason != nullptr)) {
            convertResult.errorReason->start--;
            convertResult.errorReason->end--;
            return {ASTNode::simpleNode(node, tokens, convertResult.errorReason), std::move(convertResult)};
        }
        auto tokenReader = TokenReader(std::make_shared<LexerResult>(Lexer::lex(convertResult.result)));
#ifdef CHelperTest
        Profile::push("start parsing: {}", FORMAT_ARG(utf8::utf16to8(content)));
#endif
        DEBUG_GET_NODE_BEGIN(mainNode)
        ASTNode result = Parser::parse(convertResult.result, cpack, mainNode);
        DEBUG_GET_NODE_END(mainNode)
#ifdef CHelperTest
        Profile::pop();
#endif
        return {std::move(result), std::move(convertResult)};
    }

    ASTNode NodeJsonString::getASTNode(TokenReader &tokenReader, const CPack *cpack) const {
        tokenReader.push();
        ASTNode result = tokenReader.readStringASTNode(this);
        tokenReader.pop();
        TokensView tokens = result.tokens;
        std::u16string_view str = tokens.toString();
        if (HEDLEY_UNLIKELY(str.empty())) {
            return ASTNode::simpleNode(this, tokens, ErrorReason::incomplete(tokens, u"字符串参数内容为空"));
        } else if (HEDLEY_UNLIKELY(str[0] != '"')) {
            return ASTNode::simpleNode(this, tokens, ErrorReason::contentError(tokens, fmt::format(u"字符串参数内容应该在双引号内 -> {}", str)));
        }
        std::shared_ptr<ErrorReason> errorReason;
        if (HEDLEY_LIKELY(str.size() <= 1 || str[str.size() - 1] != '"')) {
            errorReason = ErrorReason::contentError(tokens, fmt::format(u"字符串参数内容应该在双引号内 -> {}", str));
        }
        if (HEDLEY_LIKELY(!data.has_value() || data->empty())) {
            return ASTNode::simpleNode(this, tokens, errorReason);
        }
        size_t offset = tokens.getStartIndex() + 1;
        auto innerNode = getInnerASTNode(this, tokens, std::u16string(str), cpack, nodeData.get());
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
        return newResult;
    }

    bool NodeJsonString::collectIdError(const ASTNode *astNode,
                                        std::vector<std::shared_ptr<ErrorReason>> &idErrorReasons) const {
        if (HEDLEY_UNLIKELY(astNode->id == ASTNodeId::NODE_STRING_INNER)) {
            auto convertResult = JsonUtil::jsonString2String(std::u16string(astNode->tokens.toString()));
            size_t offset = astNode->tokens.getStartIndex() + 1;
            for (const auto &item: astNode->childNodes[0].getIdErrors()) {
                item->start = convertResult.convert(item->start) + offset;
                item->end = convertResult.convert(item->end) + offset;
                idErrorReasons.push_back(item);
            }
        }
        return true;
    }

}// namespace CHelper::Node