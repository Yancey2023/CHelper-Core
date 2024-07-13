//
// Created by Yancey on 2023/11/6.
//

#include "Parser.h"

namespace CHelper::Parser {

    ASTNode parse(const std::string &content, const CPack *cpack, const Node::NodeBase *mainNode) {
        TokenReader tokenReader = TokenReader(std::make_shared<LexerResult>(Lexer::lex(content)));
        Profile::push("start parsing: " + tokenReader.lexerResult->content);
        DEBUG_GET_NODE_BEGIN(mainNode)
        auto result = mainNode->getASTNode(tokenReader, cpack);
        DEBUG_GET_NODE_END(mainNode)
        Profile::pop();
        return result;
    }

    ASTNode parse(const std::string &content, const CPack *cpack) {
        return parse(content, cpack, cpack->mainNode.get());
    }

}// namespace CHelper::Parser