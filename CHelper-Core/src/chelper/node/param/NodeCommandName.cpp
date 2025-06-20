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

    bool NodeCommandName::collectIdError(const ASTNode *astNode,
                                         std::vector<std::shared_ptr<ErrorReason>> &idErrorReasons) const {
        if (HEDLEY_UNLIKELY(astNode->isError())) {
            return true;
        }
        std::u16string_view str = astNode->tokens.toString();
        for (const auto &command: *commands) {
            for (const auto &name: static_cast<NodePerCommand *>(command.get())->name) {
                if (HEDLEY_UNLIKELY(str == name)) {
                    return true;
                }
            }
        }
        idErrorReasons.push_back(ErrorReason::idError(astNode->tokens, fmt::format(u"找不到命令名 -> {}", str)));
        return true;
    }

}// namespace CHelper::Node