//
// Created by Yancey on 2023/11/6.
//

#include "Parser.h"
#include "../util/TokenUtil.h"
#include "../lexer/Lexer.h"

#include <utility>

namespace CHelper::Parser {

    static Node::NodeCommand mainNode("MAIN_NODE", "main node");

    std::pair<std::shared_ptr<std::vector<Token>>, ASTNode> parse(const std::string &content, const CPack &cpack) {
        return parse(StringReader(content, "unknown"), cpack);
    }

    std::pair<std::shared_ptr<std::vector<Token>>, ASTNode> parse(StringReader &&stringReader, const CPack &cpack) {
        std::shared_ptr<std::vector<Token>> tokens = std::make_shared<std::vector<Token>>(Lexer(stringReader).lex());
        return {tokens, parse(*tokens, cpack)};
    }

    ASTNode parse(const std::vector<Token> &tokens, const CPack &cpack) {
        return parse(TokenReader(tokens), cpack);
    }

    ASTNode parse(TokenReader &&tokenReader, const CPack &cpack) {
        VectorView <Token> tokens = {&tokenReader.tokenList, 0, tokenReader.tokenList.size()};
        Profile::push("start parsing: " + TokenUtil::toString(tokens));
        auto result = mainNode.getASTNode(tokenReader, cpack);
        Profile::pop();
        return result;
    }

} // CHelper::Parser