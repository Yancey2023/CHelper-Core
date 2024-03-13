//
// Created by Yancey on 2023/11/11.
//

#include "NodeCommandName.h"
#include "../../resources/CPack.h"
#include "../../util/TokenUtil.h"

namespace CHelper::Node {

    NodeCommandName::NodeCommandName(const std::optional<std::string> &id,
                                     const std::optional<std::string> &description,
                                     const std::vector<std::unique_ptr<Node::NodeBase>> *commands)
            : NodeBase(id, description, false),
              commands(commands) {}

    NodeCommandName::NodeCommandName(const nlohmann::json &j,
                                     const CPack &cpack)
            : NodeBase(j, true),
              commands(cpack.commands.get()) {}

    NodeType *NodeCommandName::getNodeType() const {
        return NodeType::COMMAND_NAME.get();
    }

    ASTNode NodeCommandName::getASTNode(TokenReader &tokenReader, const CPack *cpack) const {
        return tokenReader.readStringASTNode(this);
    }

    bool NodeCommandName::collectIdError(const ASTNode *astNode,
                                         std::vector<std::shared_ptr<ErrorReason>> &idErrorReasons) const {
        if (astNode->isError()) {
            return true;
        }
        std::string str = TokenUtil::toString(astNode->tokens);
        for (const auto &command: *commands) {
            for (const auto &name: ((NodePerCommand *) command.get())->name) {
                if (str == name) {
                    return true;
                }
            }
        }
        idErrorReasons.push_back(ErrorReason::idError(astNode->tokens, std::string("找不到命令名 -> ").append(str)));
        return true;
    }

    bool NodeCommandName::collectSuggestions(const ASTNode *astNode,
                                             size_t index,
                                             std::vector<Suggestions> &suggestions) const {
        std::string str = TokenUtil::toString(astNode->tokens)
                .substr(0, index - TokenUtil::getStartIndex(astNode->tokens));
        Suggestions suggestions1;
        for (const auto &command: *commands) {
            for (const auto &name: ((NodePerCommand *) command.get())->name) {
                if (StringUtil::isStartOf(name, str)) {
                    suggestions1.suggestions.emplace_back(astNode->tokens,
                                                          std::make_shared<NormalId>(name, command->description));
                }
            }
        }
        suggestions1.markFiltered();
        suggestions.push_back(std::move(suggestions1));
        return true;
    }

    void NodeCommandName::collectStructure(const ASTNode *astNode,
                                           StructureBuilder &structure,
                                           bool isMustHave) const {
        structure.append(isMustHave, "命令名");
    }

} // CHelper::Node