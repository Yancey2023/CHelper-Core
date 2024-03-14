//
// Created by Yancey on 2023/11/11.
//

#include "NodeCommand.h"

#include "../../resources/CPack.h"
#include "../../util/TokenUtil.h"

namespace CHelper::Node {

    NodeCommand::NodeCommand(const std::optional<std::string> &id,
                             const std::optional<std::string> &description,
                             const std::vector<const NodePerCommand *> &commands)
            : NodeBase(id, description, false),
              commands(commands) {}

    static std::vector<const NodePerCommand *> getCommands(const CPack &cpack) {
        std::vector<const NodePerCommand *> result;
        for (const auto &item: *cpack.commands) {
            result.push_back((NodePerCommand *) item.get());
        }
        return result;
    }

    NodeCommand::NodeCommand(const std::optional<std::string> &id,
                             const std::optional<std::string> &description,
                             [[maybe_unused]] const CPack &cpack)
            : NodeBase(id, description, false),
              commands(getCommands(cpack)) {}

    NodeCommand::NodeCommand(const nlohmann::json &j,
                             [[maybe_unused]] const CPack &cpack)
            : NodeBase(j, true),
              commands(getCommands(cpack)) {}

    NodeType *NodeCommand::getNodeType() const {
        return NodeType::COMMAND.get();
    }

    ASTNode NodeCommand::getASTNode(TokenReader &tokenReader, const CPack *cpack) const {
        tokenReader.push();
        ASTNode commandName = tokenReader.readStringASTNode(this, "commandName");
        if (commandName.tokens.size() == 0) {
            VectorView <Token> tokens = tokenReader.collect();
            return ASTNode::andNode(this, {std::move(commandName)}, tokens,
                                    ErrorReason::contentError(tokens, "命令名字为空"), "command");
        }
        std::string str = TokenUtil::toString(commandName.tokens);
        const NodePerCommand *currentCommand = nullptr;
        if (!commandName.isError()) {
            bool isBreak = false;
            for (const auto &item: commands) {
                for (const auto &item2: item->name) {
                    if (str == item2) {
                        currentCommand = item;
                        isBreak = true;
                        break;
                    }
                }
                if (isBreak) {
                    break;
                }
            }
        }
        if (currentCommand == nullptr) {
            VectorView <Token> tokens = tokenReader.collect();
            return ASTNode::andNode(this, {std::move(commandName)}, tokens, ErrorReason::contentError(
                    tokens, FormatUtil::format("命令名字不匹配，找不到名为{0}的命令", str)), "command");
        }
        ASTNode usage = currentCommand->getASTNode(tokenReader, cpack);
        return ASTNode::andNode(this, {std::move(commandName), std::move(usage)},
                                tokenReader.collect(), nullptr, "command");
    }

    std::optional<std::string> NodeCommand::collectDescription(const ASTNode *astNode, size_t index) const {
        if (astNode->childNodes.size() > 1) {
            return std::nullopt;
        }
        return description;
    }

    bool NodeCommand::collectSuggestions(const ASTNode *astNode,
                                         size_t index,
                                         std::vector<Suggestions> &suggestions) const {
        if (astNode->id != "commandName") {
            return false;
        }
        std::string str = TokenUtil::toString(astNode->tokens)
                .substr(0, index - TokenUtil::getStartIndex(astNode->tokens));
        std::vector<std::shared_ptr<NormalId>> suggestions1;
        for (const auto &item: commands) {
            for (const auto &item2: item->name) {
                if (StringUtil::isStartOf(item2, str)) {
                    suggestions1.push_back(std::make_shared<NormalId>(item2, item->description));
                }
            }
        }
        std::sort(suggestions1.begin(), suggestions1.end(),
                  [](const auto &item1, const auto &item2) {
                      return item1->name < item2->name;
                  });
        Suggestions suggestions2;
        for (const auto &item: suggestions1) {
            suggestions2.suggestions.emplace_back(astNode->tokens, item);
        }
        suggestions2.markFiltered();
        suggestions.push_back(std::move(suggestions2));
        return true;
    }

    void NodeCommand::collectStructure(const ASTNode *astNode,
                                       StructureBuilder &structure,
                                       bool isMustHave) const {
        if (astNode == nullptr || (astNode->id == "command" && astNode->tokens.size() < 2)) {
            structure.append(isMustHave, "命令");
            return;
        } else if (astNode->id == "commandName") {
            structure.appendWhiteSpace().append(TokenUtil::toString(astNode->tokens));
        }
    }

} // CHelper::Node