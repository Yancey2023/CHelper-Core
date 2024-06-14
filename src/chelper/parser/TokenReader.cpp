//
// Created by Yancey on 2023/12/15.
//

#include "TokenReader.h"
#include "../node/NodeBase.h"

namespace CHelper {

    TokenReader::TokenReader(const std::shared_ptr<std::vector<Token>> &tokenList)
        : tokenList(tokenList) {}

    bool TokenReader::ready() const {
        return index < tokenList->size();
    }

    const Token *TokenReader::peek() const {
        if (HEDLEY_UNLIKELY(!ready())) {
            return nullptr;
        }
        return &(*tokenList)[index];
    }

    const Token *TokenReader::read() {
        const Token *result = peek();
        if (HEDLEY_LIKELY(result != nullptr)) {
            skip();
        }
        return result;
    }

    const Token *TokenReader::next() {
        skip();
        return peek();
    }

    bool TokenReader::skip() {
        if (HEDLEY_UNLIKELY(!ready())) {
            return false;
        }
        index++;
        return true;
    }

    size_t TokenReader::skipWhitespace() {
        size_t start = index;
        while (ready() && peek()->type == TokenType::WHITE_SPACE) {
            skip();
        }
        return index - start;
    }

    void TokenReader::skipToLF() {
        while (ready() && peek()->type != TokenType::LF) {
            skip();
        }
    }

    /**
     * 将当前指针加入栈中
     */
    void TokenReader::push() {
        indexStack.push_back(index);
    }

    /**
     * 从栈中移除指针，不恢复指针
     */
    void TokenReader::pop() {
        indexStack.pop_back();
    }

    /**
     * 从栈中移除并获取最后指针，不恢复指针
     */
    size_t TokenReader::getAndPopLastIndex() {
        size_t size = indexStack.size();
        if (HEDLEY_UNLIKELY(size == 0)) {
            return 0;
        }
        size_t result = indexStack[size - 1];
        pop();
        return result;
    }


    /**
     * 从栈中移除指针，恢复指针
     */
    void TokenReader::restore() {
        index = getAndPopLastIndex();
    }

    /**
     * 收集栈中最后一个指针位置到当前指针的token，从栈中移除指针，不恢复指针
     */
    VectorView<Token> CHelper::TokenReader::collect() {
        return {tokenList, getAndPopLastIndex(), index};
    }

    ASTNode TokenReader::readSimpleASTNode(const Node::NodeBase *node,
                                           TokenType::TokenType type,
                                           const std::string &requireType,
                                           const std::string &astNodeId,
                                           std::shared_ptr<ErrorReason> (*check)(const std::string &str,
                                                                                 const VectorView<Token> &tokens)) {
        skipWhitespace();
        push();
        const Token *token = read();
        VectorView<Token> tokens = collect();
        std::shared_ptr<ErrorReason> errorReason;
        if (HEDLEY_UNLIKELY(token == nullptr)) {
            errorReason = ErrorReason::incomplete(tokens, FormatUtil::format(
                                                                  "命令不完整，需要的参数类型为{0}", requireType));
        } else if (HEDLEY_UNLIKELY(token->type != type)) {
            errorReason = ErrorReason::typeError(tokens, FormatUtil::format(
                                                                 "类型不匹配，正确的参数类型为{0}，但当前参数类型为{1}",
                                                                 requireType, TokenType::getName(token->type)));
        } else {
            errorReason = check == nullptr ? nullptr : check(token->content, tokens);
        }
        return ASTNode::simpleNode(node, tokens, errorReason, astNodeId);
    }

    ASTNode TokenReader::readStringASTNode(const Node::NodeBase *node,
                                           const std::string &astNodeId) {
        return readSimpleASTNode(node, TokenType::STRING, "字符串类型", astNodeId);
    }

    ASTNode TokenReader::readIntegerASTNode(const Node::NodeBase *node,
                                            const std::string &astNodeId) {
        return readSimpleASTNode(
                node, TokenType::NUMBER, "整数类型", astNodeId,
                [](const std::string &str, const VectorView<Token> &tokens) -> std::shared_ptr<ErrorReason> {
                    for (const auto &ch: str) {
                        if (HEDLEY_UNLIKELY(ch == '.')) {
                            return ErrorReason::contentError(
                                    tokens, "类型不匹配，正确的参数类型为整数，但当前参数类型为小数");
                        }
                    }
                    return nullptr;
                });
    }

    ASTNode TokenReader::readFloatASTNode(const Node::NodeBase *node,
                                          const std::string &astNodeId) {
        return readSimpleASTNode(
                node, TokenType::NUMBER, "数字类型", astNodeId,
                [](const std::string &str, const VectorView<Token> &tokens) -> std::shared_ptr<ErrorReason> {
                    bool isHavePoint = false;
                    for (const auto &ch: str) {
                        if (HEDLEY_LIKELY(ch != '.')) {
                            continue;
                        }
                        if (HEDLEY_UNLIKELY(isHavePoint)) {
                            return ErrorReason::contentError(tokens, "数字格式错误");
                        }
                        isHavePoint = true;
                    }
                    return nullptr;
                });
    }

    ASTNode TokenReader::readSymbolASTNode(const Node::NodeBase *node,
                                           const std::string &astNodeId) {
        return readSimpleASTNode(node, TokenType::SYMBOL, "符号类型", astNodeId);
    }

    ASTNode TokenReader::readUntilWhitespace(const Node::NodeBase *node,
                                             const std::string &astNodeId) {
        push();
        while (ready()) {
            TokenType::TokenType tokenType = peek()->type;
            if (HEDLEY_UNLIKELY(tokenType == TokenType::WHITE_SPACE || tokenType == TokenType::LF)) {
                break;
            }
            skip();
        }
        return ASTNode::simpleNode(node, collect(), nullptr, astNodeId);
    }

    ASTNode TokenReader::readStringOrNumberASTNode(const Node::NodeBase *node,
                                                   const std::string &astNodeId) {
        push();
        while (ready()) {
            TokenType::TokenType tokenType = peek()->type;
            if (HEDLEY_UNLIKELY(tokenType == TokenType::SYMBOL || tokenType == TokenType::WHITE_SPACE || tokenType == TokenType::LF)) {
                break;
            }
            skip();
        }
        return ASTNode::simpleNode(node, collect(), nullptr, astNodeId);
    }

}// namespace CHelper