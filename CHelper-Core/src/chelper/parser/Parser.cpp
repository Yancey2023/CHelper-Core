//
// Created by Yancey on 2023/11/6.
//

#include <chelper/node/NodeType.h>
#include <chelper/parser/Parser.h>

namespace CHelper::Parser {

    ASTNode parse(const std::u16string &content, const CPack *cpack, const Node::NodeBase *mainNode) {
        TokenReader tokenReader = TokenReader(std::make_shared<LexerResult>(Lexer::lex(content)));
#ifdef CHelperTest 
        Profile::push("start parsing: {}", tokenReader.lexerResult->content);
#endif
        DEBUG_GET_NODE_BEGIN(mainNode)
        auto result = mainNode->getASTNode(tokenReader, cpack);
        DEBUG_GET_NODE_END(mainNode)
#ifdef CHelperTest 
        Profile::pop();
#endif
        return result;
    }

    ASTNode parse(const std::u16string &content, const CPack *cpack) {
        return parse(content, cpack, cpack->mainNode.get());
    }

}// namespace CHelper::Parser