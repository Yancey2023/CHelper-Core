//
// Created by Yancey on 2023/12/15.
//

#ifndef CHELPER_TOKENREADER_H
#define CHELPER_TOKENREADER_H

#include "pch.h"
#include "../lexer/Token.h"
#include "../util/VectorView.h"
#include "ASTNode.h"

namespace CHelper {

    namespace Node {

        class NodeBase;

    } // Node

    class TokenReader {
    public:
        const std::shared_ptr<std::vector<Token>> tokenList;
        size_t index = 0;
        std::vector<size_t> indexStack;

        explicit TokenReader(const std::shared_ptr<std::vector<Token>> &tokenList);

        [[nodiscard]] bool ready() const;

        [[nodiscard]] const Token *peek() const;

        [[nodiscard]] const Token *read();

        [[nodiscard]] const Token *next();

        bool skip();

        size_t skipWhitespace();

        void skipToLF();

        void push();

        void pop();

        [[nodiscard]] size_t getAndPopLastIndex();

        void restore();

        [[nodiscard]] VectorView <Token> collect();

    private:
        ASTNode getSimpleASTNode(const Node::NodeBase *node,
                                 TokenType::TokenType type,
                                 const std::string &requireType,
                                 const std::string &astNodeId,
                                 std::shared_ptr<ErrorReason>(*check)(const std::string &str,
                                                                      const VectorView <Token> &tokens));

    public:
        ASTNode getStringASTNode(const Node::NodeBase *node,
                                 const std::string &astNodeId = "");

        ASTNode getIntegerASTNode(const Node::NodeBase *node,
                                  const std::string &astNodeId = "");

        ASTNode getFloatASTNode(const Node::NodeBase *node,
                                const std::string &astNodeId = "");

        ASTNode getSymbolASTNode(const Node::NodeBase *node,
                                 const std::string &astNodeId = "");

    };

} // CHelper

#endif //CHELPER_TOKENREADER_H
