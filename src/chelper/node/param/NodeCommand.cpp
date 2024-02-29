//
// Created by Yancey on 2023/11/11.
//

#include "NodeCommand.h"

#include "../../resources/CPack.h"

namespace CHelper::Node {

    NodeCommand::NodeCommand(const std::optional<std::string> &id,
                             const std::optional<std::string> &description,
                             const std::shared_ptr<std::vector<std::shared_ptr<NodeBase>>> &childNodes)
            : NodeBase(id, description, false),
              nodeCommand(std::make_shared<NodeOr>("COMMAND", "命令", childNodes, true, "command_or")) {}

    NodeCommand::NodeCommand(const nlohmann::json &j,
                             const CPack &cpack)
            : NodeBase(j, true),
              nodeCommand(std::make_shared<NodeOr>("COMMAND", "命令", cpack.commands, true, "command_or")) {}

    std::shared_ptr<NodeType> NodeCommand::getNodeType() const {
        return NodeType::COMMAND;
    }

    ASTNode NodeCommand::getASTNode(TokenReader &tokenReader, const CPack &cpack) const {
        DEBUG_GET_NODE_BEGIN(nodeCommand)
        auto result = getByChildNode(tokenReader, cpack, {nodeCommand}, "command");
        DEBUG_GET_NODE_END(nodeCommand)
        return result;
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