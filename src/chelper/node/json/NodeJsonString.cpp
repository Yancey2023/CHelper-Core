//
// Created by Yancey on 2024/2/28.
//

#include "NodeJsonString.h"
#include "../../util/TokenUtil.h"
#include "../../parser/Parser.h"
#include "../../lexer/Lexer.h"

namespace CHelper::Node {

    NodeJsonString::NodeJsonString(const std::optional<std::string> &id,
                                   const std::optional<std::string> &description,
                                   const std::shared_ptr<NodeBase> &data)
            : NodeBase(id, description, false),
              data(data) {}

    static std::shared_ptr<NodeBase> getDataFromCpack(const nlohmann::json &j,
                                                      const CPack &cpack) {
        if (!j.contains("data")) {
            return nullptr;
        }
        auto childNodes = std::make_shared<std::vector<std::shared_ptr<NodeBase>>>();
        for (const auto &item: j.at("data")) {
            childNodes->push_back(NodeJsonString::getNodeFromJson(item, cpack));
        }
        return std::make_shared<NodeOr>("JSON_STRING_DATA", "JSON字符串内容", childNodes, false);
    }

    NodeJsonString::NodeJsonString(const nlohmann::json &j,
                                   const CPack &cpack)
            : NodeBase(j, false),
              data(getDataFromCpack(j, cpack)) {}

    std::shared_ptr<NodeType> NodeJsonString::getNodeType() const {
        return NodeType::JSON_STRING;
    }

    void NodeJsonString::toJson(nlohmann::json &j) const {
        NodeBase::toJson(j);
        j["data"] = data;
    }

    class ConvertResult {
    public:
        std::string result;
        std::shared_ptr<ErrorReason> errorReason;
        std::vector<size_t> indexConvertList;

        size_t convert(size_t index) {
            return indexConvertList[index];
        }

    };

    static std::string string2jsonString(const std::string &input) {
        std::string result;
        StringReader stringReader(input, "unknown");
        while (true) {
            char ch = stringReader.peek();
            switch (ch) {
                case EOF:
                    return result;
                case '\"':
                case '\\':
                case '/':
                case '\b':
                case '\f':
                case '\n':
                case '\r':
                case '\t':
                    result.push_back('\\');
                    result.push_back(ch);
                    stringReader.skip();
                    break;
                default:
                    result.push_back(ch);
                    break;
            }
            stringReader.skip();
        }
    }

    static ConvertResult jsonString2String(const std::string &input) {
        std::string input0;
        if (input.size() > 1 && input[input.size() - 1] == '"') {
            input0 = input.substr(1, input.length() - 2);
        } else {
            input0 = input.substr(1, input.length() - 1);
        }
        ConvertResult result;
        result.indexConvertList.push_back(0);
        StringReader stringReader(input0, "unknown");
        int unicodeValue;
        std::string escapeSequence;
        while (true) {
            if (result.errorReason != nullptr) {
                break;
            }
            char ch = stringReader.peek();
            if (ch == EOF) {
                break;
            }
            if (ch != '\\') {
                result.result.push_back(ch);
                stringReader.skip();
                result.indexConvertList.push_back(stringReader.pos.index);
                continue;
            }
            //转义字符
            ch = stringReader.next();
            switch (ch) {
                case EOF:
                    result.errorReason = ErrorReason::incomplete(input.length(), input.length() + 1,
                                                                 "转义字符缺失后半部分");
                    break;
                case '\"':
                case '\\':
                case '/':
                case '\b':
                case '\f':
                case '\n':
                case '\r':
                case '\t':
                    result.result.push_back(ch);
                    stringReader.skip();
                    result.indexConvertList.push_back(stringReader.pos.index);
                    break;
                case 'u':
                    for (int i = 0; i < 4; ++i) {
                        ch = stringReader.next();
                        if (ch == EOF) {
                            result.errorReason = ErrorReason::contentError(input.length(), input.length() + 1,
                                                                           "字符串转义缺失后半部分 -> \\u" +
                                                                           escapeSequence);
                            break;
                        }
                        escapeSequence.push_back(ch);
                    }
                    if (escapeSequence.length() < 4) {
                        break;
                    }
                    if (std::any_of(escapeSequence.begin(), escapeSequence.end(),
                                    [&result, &input, &escapeSequence](const auto &item) {
                                        bool notHex = std::isxdigit(item) != 0;
                                        if (notHex) {
                                            result.errorReason = ErrorReason::incomplete(
                                                    input.length(), input.length() + escapeSequence.length() + 2,
                                                    FormatUtil::format(
                                                            "字符串转义出现非法字符{0} -> \\u{1}",
                                                            item, escapeSequence));
                                        }
                                        return notHex;
                                    })) {
                        break;
                    }
                    unicodeValue = std::stoi(escapeSequence, nullptr, 16);
                    if (unicodeValue <= 0 || unicodeValue > 0x10FFFF) {
                        result.errorReason = ErrorReason::contentError(input.length(), input.length() + 1,
                                                                       "字符串转义的Unicode值无效 -> \\u" +
                                                                       escapeSequence);
                        break;
                    }
                    escapeSequence.clear();
                    if (unicodeValue <= 0x7F) {
                        result.result.push_back(static_cast<char>(unicodeValue));
                    } else if (unicodeValue <= 0x7FF) {
                        result.result.push_back(
                                static_cast<char>(0xC0u | (static_cast<unsigned int>(unicodeValue) >> 6u)));
                        result.result.push_back(
                                static_cast<char>(0x80u | (static_cast<unsigned int>(unicodeValue) & 0x3Fu)));
                    } else if (unicodeValue <= 0xFFFF) {
                        result.result.push_back(
                                static_cast<char>(0xE0u | (static_cast<unsigned int>(unicodeValue) >> 12u)));
                        result.result.push_back(
                                static_cast<char>(0x80u | ((static_cast<unsigned int>(unicodeValue) >> 6u) & 0x3Fu)));
                        result.result.push_back(
                                static_cast<char>(0x80u | (static_cast<unsigned int>(unicodeValue) & 0x3Fu)));
                    } else {
                        result.result.push_back(
                                static_cast<char>(0xF0u | (static_cast<unsigned int>(unicodeValue) >> 18u)));
                        result.result.push_back(
                                static_cast<char>(0x80u | ((static_cast<unsigned int>(unicodeValue) >> 12u) & 0x3Fu)));
                        result.result.push_back(
                                static_cast<char>(0x80u | ((static_cast<unsigned int>(unicodeValue) >> 6u) & 0x3Fu)));
                        result.result.push_back(
                                static_cast<char>(0x80u | (static_cast<unsigned int>(unicodeValue) & 0x3Fu)));
                    }
                    stringReader.skip();
                    result.indexConvertList.push_back(stringReader.pos.index);
                    break;
                default:
                    result.errorReason = ErrorReason::contentError(input.length(), input.length() + 1,
                                                                   FormatUtil::format("未知的转义字符 -> \\{0}", ch));
                    break;
            }
        }
        return result;
    }

    static std::pair<ASTNode, ConvertResult> getInnerASTNode(const NodeJsonString *node,
                                                             const VectorView <Token> &tokens,
                                                             const std::string &content,
                                                             const CPack &cpack,
                                                             const std::shared_ptr<NodeBase> &mainNode) {
        auto contentResult = jsonString2String(content);
        if (contentResult.errorReason != nullptr) {
            return {ASTNode::simpleNode(node, tokens, contentResult.errorReason), contentResult};
        }
        auto newStringReader = StringReader(contentResult.result, "unknown");
        auto newTokens = std::make_shared<std::vector<Token>>(Lexer::lex(newStringReader));
        auto tokenReader = TokenReader(newTokens);
        Profile::push("start parsing: " + content);
        DEBUG_GET_NODE_BEGIN(mainNode)
        auto result = mainNode->getASTNode(tokenReader, cpack);
        DEBUG_GET_NODE_END(mainNode)
        Profile::pop();
        return {result, contentResult};
    }

    ASTNode NodeJsonString::getASTNode(TokenReader &tokenReader, const CPack &cpack) const {
        tokenReader.push();
        ASTNode result = tokenReader.readStringASTNode(this);
        tokenReader.pop();
        std::string str = TokenUtil::toString(result.tokens);
        if (str.empty()) {
            return ASTNode::simpleNode(this, result.tokens, ErrorReason::incomplete(
                    result.tokens, "字符串参数内容为空"));
        } else if (str[0] != '"') {
            return ASTNode::simpleNode(this, result.tokens, ErrorReason::contentError(
                    result.tokens, "字符串参数内容应该在双引号内 -> " + str));
        }
        if (data == nullptr) {
            return result;
        }
        std::shared_ptr<ErrorReason> errorReason;
        if (str.size() <= 1 || str[str.size() - 1] != '"') {
            errorReason = ErrorReason::contentError(result.tokens, "字符串参数内容应该在双引号内 -> " + str);
        }
        size_t offset = TokenUtil::getStartIndex(result.tokens) + 1;
        auto innerNode = getInnerASTNode(this, result.tokens, str, cpack, data);
        ASTNode newResult = ASTNode::andNode(this, {innerNode.first}, result.tokens, errorReason, "inner");
        if (errorReason == nullptr) {
            size_t size = newResult.errorReasons.size();
            for (int i = 0; i < size; ++i) {
                ErrorReason newErrorReason = ErrorReason(*newResult.errorReasons[i]);
                newErrorReason.start = innerNode.second.convert(newErrorReason.start) + offset;
                newErrorReason.end = innerNode.second.convert(newErrorReason.end) + offset;
                newResult.errorReasons[i] = std::make_shared<ErrorReason>(newErrorReason);
            }
        }
        return newResult;
    }

    bool NodeJsonString::collectIdError(const ASTNode *astNode,
                                        std::vector<std::shared_ptr<ErrorReason>> &idErrorReasons) const {
        if (astNode->id != "inner") {
            return true;
        }
        auto convertResult = jsonString2String(TokenUtil::toString(astNode->tokens));
        size_t offset = TokenUtil::getStartIndex(astNode->tokens) + 1;
        for (const auto &item: astNode->childNodes[0].getIdErrors()) {
            item->start = convertResult.convert(item->start) + offset;
            item->end = convertResult.convert(item->end) + offset;
            idErrorReasons.push_back(item);
        }
        return true;
    }

    bool NodeJsonString::collectSuggestions(const ASTNode *astNode,
                                            size_t index,
                                            std::vector<Suggestion> &suggestions) const {
        if (astNode->id != "inner") {
            return true;
        }
        auto convertResult = jsonString2String(TokenUtil::toString(astNode->tokens));
        size_t offset = TokenUtil::getStartIndex(astNode->tokens) + 1;
        for (auto &item: astNode->childNodes[0].getSuggestions(index - offset)) {
            item.start = convertResult.convert(item.start) + offset;
            item.end = convertResult.convert(item.end) + offset;
            item.content = std::make_shared<NormalId>(string2jsonString(item.content->name), item.content->description);
            suggestions.push_back(item);
        }
        return true;
    }

} // CHelper::Node