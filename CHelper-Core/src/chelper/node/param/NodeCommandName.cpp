//
// Created by Yancey on 2023/11/11.
//

#include <chelper/node/param/NodeCommandName.h>
#include <chelper/resources/CPack.h>

namespace CHelper::Node {

    void NodeCommandName::init(const CPack &cpack) {
        commands = cpack.commands.get();
    }

    NodeTypeId::NodeTypeId NodeCommandName::getNodeType() const {
        return NodeTypeId::COMMAND_NAME;
    }

    ASTNode NodeCommandName::getASTNode(TokenReader &tokenReader, const CPack *cpack) const {
        return tokenReader.readStringASTNode(this);
    }

}// namespace CHelper::Node