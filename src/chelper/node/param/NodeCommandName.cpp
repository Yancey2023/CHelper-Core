//
// Created by Yancey on 2023/11/11.
//

#include "NodeCommandName.h"
#include "../../resources/CPack.h"

namespace CHelper::Node {

    NODE_TYPE("COMMAND_NAME", NodeCommandName)

    NodeCommandName::NodeCommandName(const std::optional<std::string> &id,
                                     const std::optional<std::string> &description)
            : NodeBase(id, description) {}

    NodeCommandName::NodeCommandName(const nlohmann::json &j,
                                     const CPack &cpack)
            : NodeBase(j, cpack) {}

    ASTNode NodeCommandName::getASTNode(TokenReader &tokenReader, const CPack &cpack) const {
        return getStringASTNode(tokenReader);
    }

    bool NodeCommandName::collectIdError(const ASTNode *astNode,
                                         const CPack &cpack,
                                         std::vector<std::shared_ptr<ErrorReason>> &idErrorReasons) const {
        if (astNode->isError()) {
            return true;
        }
        std::string str = astNode->tokens.size() == 0 ? "" : astNode->tokens[0].content;
        for (const auto &command: cpack.commands) {
            for (const auto &name: command->name) {
                if(str == name){
                    return true;
                }
            }
        }
        idErrorReasons.push_back(ErrorReason::idError(astNode->tokens, "找不到命令名 -> " + str));
        return true;
    }

    bool NodeCommandName::collectSuggestions(const ASTNode *astNode, const CPack &cpack,
                                             std::vector<Suggestion> &suggestions) const {
        if (astNode->isError()) {
            return true;
        }
        std::string str = astNode->tokens.size() == 0 ? "" : astNode->tokens[0].content;
        for (const auto &command: cpack.commands) {
            for (const auto &name: command->name){
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