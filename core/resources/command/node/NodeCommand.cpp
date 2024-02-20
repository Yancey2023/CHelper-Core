//
// Created by Yancey666 on 2023/11/11.
//

#include "NodeCommand.h"

#include "../../CPack.h"

namespace CHelper::Node {

    NODE_TYPE("COMMAND", NodeCommand)

    NodeCommand::NodeCommand(const std::optional<std::string> &id,
                             const std::optional<std::string> &description,
                             const std::vector<std::shared_ptr<Node::NodeBase>> &commands)
            : NodeBase(id, description) {}

    NodeCommand::NodeCommand(const nlohmann::json &j,
                             const CPack &cpack)
            : NodeBase(j, cpack) {}

    ASTNode NodeCommand::getASTNode(TokenReader &tokenReader, const CPack &cpack) const {
        std::vector<ASTNode> childASTNodes;
        childASTNodes.reserve(cpack.commands.size());
        for (const auto &item: cpack.commands) {
            tokenReader.push();
            childASTNodes.push_back(item->getASTNode(tokenReader, cpack));
            tokenReader.restore();
        }
        tokenReader.push();
        while (tokenReader.ready()) {
            if (tokenReader.read()->type == TokenType::LF) {
                break;
            }
        }
        return ASTNode::orNode(this, childASTNodes, tokenReader.collect());
    }

} // CHelper::Node