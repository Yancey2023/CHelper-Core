//
// Created by Yancey on 2023/11/11.
//

#include "NodeCommand.h"

#include "../../resources/CPack.h"
#include "../../util/TokenUtil.h"
#include "../util/NodeSingleSymbol.h"

namespace CHelper::Node {

    static std::unique_ptr<Node::NodeSingleSymbol> nodeCommandStart = std::make_unique<Node::NodeSingleSymbol>(
            "COMMAND_START", "命令开始字符", '/');

    NodeCommand::NodeCommand(const std::optional<std::string> &id,
                             const std::optional<std::string> &description,
                             const std::vector<std::unique_ptr<Node::NodeBase>> *commands)
            : NodeBase(id, description, false),
              commands(commands) {}

    NodeCommand::NodeCommand(const std::optional<std::string> &id,
                             const std::optional<std::string> &description,
                             [[maybe_unused]] const CPack &cpack)
            : NodeBase(id, description, false),
              commands(cpack.commands.get()) {}

    NodeCommand::NodeCommand(const nlohmann::json &j,
                             [[maybe_unused]] const CPack &cpack)
            : NodeBase(j, true),
              commands(cpack.commands.get()) {}

    NodeType *NodeCommand::getNodeType() const {
        return NodeType::COMMAND.get();
    }

    ASTNode NodeCommand::getASTNode(TokenReader &tokenReader, const CPack *cpack) const {
        tokenReader.push();
        ASTNode commandStart = nodeCommandStart->getASTNode(tokenReader, cpack);
        if (commandStart.isError()) {
            tokenReader.restore();
            tokenReader.push();
        }
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
            for (const auto &item: *commands) {
                currentCommand = (NodePerCommand *) item.get();
                for (const auto &item2: currentCommand->name) {
                    if (str == item2) {
                        isBreak = true;
                        break;
                    }
                }
                if (isBreak) {
                    break;
                }
                currentCommand = nullptr;
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
        if (astNode->id == "commandName") {
            return "命令的名字";
        } else {
            return std::nullopt;
        }
    }

    bool NodeCommand::collectSuggestions(const ASTNode *astNode,
                                         size_t index,
                                         std::vector<Suggestions> &suggestions) const {
        if (astNode->id != "commandName") {
            return false;
        }
        std::string str = TokenUtil::toString(astNode->tokens)
                .substr(0, index - TokenUtil::getStartIndex(astNode->tokens));
        std::vector<std::shared_ptr<NormalId>> nameStartOf, nameContain, descriptionContain;
        for (const auto &item: *commands) {
            //通过名字进行搜索
            bool flag = false;
            for (const auto &item2: ((NodePerCommand *) item.get())->name) {
                size_t index1 = item2.find(str);
                if (index1 != std::string::npos) {
                    if (index1 == 0) {
                        nameStartOf.push_back(std::make_shared<NormalId>(item2, item->description));
                    } else {
                        nameContain.push_back(std::make_shared<NormalId>(item2, item->description));
                    }
                }
                flag = true;
            }
            if (flag) {
                continue;
            }
            //通过介绍进行搜索
            if (item->description.has_value()) {
                size_t index2 = item->description.value().find(str);
                if (index2 != std::string::npos) {
                    for (const auto &item2: ((NodePerCommand *) item.get())->name) {
                        descriptionContain.push_back(std::make_shared<NormalId>(item2, item->description));
                    }
                }
            }
        }
        std::vector<std::shared_ptr<NormalId>> suggestions1;
        suggestions1.reserve(nameStartOf.size() + nameContain.size() + descriptionContain.size());
        suggestions1.insert(suggestions1.end(), nameStartOf.begin(), nameStartOf.end());
        suggestions1.insert(suggestions1.end(), nameContain.begin(), nameContain.end());
        suggestions1.insert(suggestions1.end(), descriptionContain.begin(), descriptionContain.end());
        std::sort(suggestions1.begin(), suggestions1.end(),
                  [](const auto &item1, const auto &item2) {
                      return item1->name < item2->name;
                  });
        Suggestions suggestions2;
        size_t start = TokenUtil::getStartIndex(astNode->tokens);
        size_t end = TokenUtil::getStartIndex(astNode->tokens);
        std::transform(suggestions1.begin(), suggestions1.end(),
                       std::back_inserter(suggestions2.suggestions),
                       [&start, &end](const auto &item) {
                           return Suggestion(start, end, item);
                       });
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