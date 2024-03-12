//
// Created by Yancey on 2023/11/11.
//

#include "NodeCommand.h"

#include "../../resources/CPack.h"

namespace CHelper::Node {

    NodeCommand::NodeCommand(const std::optional<std::string> &id,
                             const std::optional<std::string> &description,
                             const std::vector<const NodeBase *> &childNodes)
            : NodeBase(id, description, false),
              nodeCommand("COMMAND", "命令", childNodes, true, "command_or") {}

    static std::vector<const NodeBase *> getCommands(const CPack &cpack) {
        std::vector<const NodeBase *> result;
        for (const auto &item: *cpack.commands) {
            result.push_back(item.get());
        }
        return result;
    }

    NodeCommand::NodeCommand(const nlohmann::json &j,
                             const CPack &cpack)
            : NodeBase(j, true),
              nodeCommand("COMMAND", "命令", getCommands(cpack), true, "command_or") {}

    NodeType *NodeCommand::getNodeType() const {
        return NodeType::COMMAND.get();
    }

    ASTNode NodeCommand::getASTNode(TokenReader &tokenReader, const CPack *cpack) const {
        ASTNode node = nodeCommand.getASTNode(tokenReader, cpack);
        return ASTNode::andNode(this, {node}, node.tokens, nullptr, "command");
    }

    std::optional<std::string> NodeCommand::collectDescription(const ASTNode *astNode, size_t index) const {
        if (astNode->tokens.start == index) {
            return description;
        }
        return std::nullopt;
    }

    void NodeCommand::collectStructure(const ASTNode *astNode,
                                       StructureBuilder &structure,
                                       bool isMustHave) const {
        if (astNode != nullptr && astNode->tokens.size() > 1) {
            return;
        }
        structure.append(isMustHave, "命令");
    }

} // CHelper::Node