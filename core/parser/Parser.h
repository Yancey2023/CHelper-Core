//
// Created by Yancey666 on 2023/11/6.
//

#ifndef CHELPER_PARSER_H
#define CHELPER_PARSER_H


#include "pch.h"
#include "ASTNode.h"
#include "../resources/CPack.h"
#include "TokenReader.h"
#include "../resources/command/node/NodeCommand.h"

namespace CHelper {

    class Parser {
        const CPack& cpack;
        TokenReader tokenReader;
        Node::NodeCommand mainNode;

    public:
        Parser(TokenReader tokenReader, const CPack &cpack);

        ASTNode parse();
    };

} // CHelper

#endif //CHELPER_PARSER_H
