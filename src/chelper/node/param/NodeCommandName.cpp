//
// Created by Yancey on 2023/11/11.
//

#include "NodeCommandName.h"
#include "../../resources/CPack.h"

namespace CHelper::Node {

    NodeCommandName::NodeCommandName(const std::optional<std::string> &id,
                                     const std::optional<std::string> &description,
                                     const std::shared_ptr<std::vector<std::shared_ptr<Node::NodeBase>>> &commands)
            : NodeBase(id, description),
              commands(commands) {}

    NodeCommandName::NodeCommandName(const nlohmann::json &j,
                                     const CPack &cpack)
            : NodeBase(j, cpack),
              commands(cpack.commands) {}

    NodeType NodeCommandName::getNodeType() const {
        return NodeType::COMMAND_NAME;
    }

    ASTNode NodeCommandName::getASTNode(TokenReader &tokenReader) const {
        return getStringASTNode(tokenReader);
    }

    bool NodeCommandName::collectIdError(const ASTNode *astNode,
                                         std::vector<std::shared_ptr<ErrorReason>> &idErrorReasons) const {
        if (astNode->isError()) {
            return true;
        }
        std::string str = astNode->tokens.size() == 0 ? "" : astNode->tokens[0].content;
        for (const auto &command: *commands) {
            for (const auto &name: std::static_pointer_cast<Node::NodePerCommand>(command)->name) {
                if (str == name) {
                    return true;
                }
            }
        }
        idErrorReasons.push_back(ErrorReason::idError(astNode->tokens, "找不到命令名 -> " + str));
        return true;
    }

    bool NodeCommandName::collectSuggestions(const ASTNode *astNode, size_t index,
                                             std::vector<Suggestion> &suggestions) const {
        if (astNode->isError()) {
            return true;
        }
        std::string str = astNode->tokens.size() == 0 ? "" : astNode->tokens[0].content.substr(0, index);
        for (const auto &command: *commands) {
            for (const auto &name: std::static_pointer_cast<Node::NodePerCommand>(command)->name) {
                if (StringUtil::isStartOf(name, str)) {
                    suggestions.emplace_back(astNode->tokens, std::make_shared<NormalId>(name, command->description));
                }
            }
        }
        return true;
    }

    void NodeCommandName::collectStructure(const ASTNode *astNode,
                                           StructureBuilder &structure,
                                           bool isMustHave) const {
        structure.append(isMustHave, "命令名");
    }

} // CHelper::Node