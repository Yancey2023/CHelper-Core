//
// Created by Yancey on 2023/11/11.
//

#include <chelper/node/param/NodeCommand.h>
#include <chelper/node/util/NodeSingleSymbol.h>
#include <chelper/resources/CPack.h>

namespace CHelper::Node {

    static std::unique_ptr<Node::NodeSingleSymbol> nodeCommandStart = std::make_unique<Node::NodeSingleSymbol>(
            u"COMMAND_START", u"命令开始字符", u'/');

    NodeCommand::NodeCommand(const std::optional<std::u16string> &id,
                             const std::optional<std::u16string> &description,
                             std::vector<std::unique_ptr<Node::NodePerCommand>> *commands)
        : NodeBase(id, description, false),
          commands(commands) {}

    void NodeCommand::init(const CPack &cpack) {
        commands = cpack.commands.get();
    }

    NodeTypeId::NodeTypeId NodeCommand::getNodeType() const {
        return NodeTypeId::COMMAND;
    }

    ASTNode NodeCommand::getASTNode(TokenReader &tokenReader, const CPack *cpack, void *private_data) const {
        tokenReader.push();
        ASTNode commandStart = nodeCommandStart->getASTNode(tokenReader, cpack);
        if (HEDLEY_UNLIKELY(commandStart.isError())) {
            tokenReader.restore();
            tokenReader.push();
        }
        ASTNode commandName = tokenReader.readStringASTNode(this, ASTNodeId::NODE_COMMAND_COMMAND_NAME);
        if (HEDLEY_UNLIKELY(commandName.tokens.size() == 0)) {
            TokensView tokens = tokenReader.collect();
            return ASTNode::andNode(this, {std::move(commandName)}, tokens, ErrorReason::contentError(tokens, u"命令名字为空"), ASTNodeId::NODE_COMMAND_COMMAND);
        }
        std::u16string_view str = commandName.tokens.toString();
        const NodePerCommand *currentCommand = nullptr;
        if (HEDLEY_LIKELY(!commandName.isError())) {
            bool isBreak = false;
            for (const auto &item: *commands) {
                currentCommand = (NodePerCommand *) item.get();
                for (const auto &item2: currentCommand->name) {
                    if (HEDLEY_UNLIKELY(str == item2)) {
                        isBreak = true;
                        break;
                    }
                }
                if (HEDLEY_UNLIKELY(isBreak)) {
                    break;
                }
                currentCommand = nullptr;
            }
        }
        if (HEDLEY_UNLIKELY(currentCommand == nullptr)) {
            TokensView tokens = tokenReader.collect();
            return ASTNode::andNode(this, {std::move(commandName)}, tokens, ErrorReason::contentError(tokens, fmt::format(u"命令名字不匹配，找不到名为{}的命令", str)), ASTNodeId::NODE_COMMAND_COMMAND);
        }
        ASTNode usage = currentCommand->getASTNode(tokenReader, cpack);
        return ASTNode::andNode(this, {std::move(commandName), std::move(usage)},
                                tokenReader.collect(), nullptr, ASTNodeId::NODE_COMMAND_COMMAND);
    }

    std::optional<std::u16string> NodeCommand::collectDescription(const ASTNode *astNode, size_t index) const {
        if (HEDLEY_UNLIKELY(astNode->id == ASTNodeId::NODE_COMMAND_COMMAND_NAME)) {
            return u"命令的名字";
        } else {
            return std::nullopt;
        }
    }

    bool NodeCommand::collectSuggestions(const ASTNode *astNode,
                                         size_t index,
                                         std::vector<Suggestions> &suggestions) const {
        if (HEDLEY_UNLIKELY(astNode->id != ASTNodeId::NODE_COMMAND_COMMAND_NAME)) {
            return false;
        }
        if (HEDLEY_LIKELY(index == 0 && astNode->tokens.isEmpty())) {
            suggestions.push_back(Suggestions::singleSymbolSuggestion({0, 0, false, nodeCommandStart->normalId}));
        }
        std::u16string_view str = astNode->tokens.toString()
                                       .substr(0, index - astNode->tokens.getStartIndex());
        std::vector<std::shared_ptr<NormalId>> nameStartOf, nameContain, descriptionContain;
        for (const auto &item: *commands) {
            //通过名字进行搜索
            bool flag = false;
            for (const auto &item2: ((NodePerCommand *) item.get())->name) {
                size_t index1 = item2.find(str);
                if (HEDLEY_UNLIKELY(index1 != std::u16string::npos)) {
                    if (HEDLEY_UNLIKELY(index1 == 0)) {
                        nameStartOf.push_back(NormalId::make(item2, item->description));
                    } else {
                        nameContain.push_back(NormalId::make(item2, item->description));
                    }
                    flag = true;
                }
            }
            if (HEDLEY_UNLIKELY(flag)) {
                continue;
            }
            //通过介绍进行搜索
            if (HEDLEY_UNLIKELY(item->description.has_value() &&
                                item->description.value().find(str) != std::u16string::npos)) {
                for (const auto &item2: ((NodePerCommand *) item.get())->name) {
                    descriptionContain.push_back(NormalId::make(item2, item->description));
                }
            }
        }
        auto compare = [](const auto &item1, const auto &item2) {
            return item1->name < item2->name;
        };
        std::sort(nameStartOf.begin(), nameStartOf.end(), compare);
        std::sort(nameContain.begin(), nameContain.end(), compare);
        std::sort(descriptionContain.begin(), descriptionContain.end(), compare);
        std::vector<std::shared_ptr<NormalId>> suggestions1;
        suggestions1.reserve(nameStartOf.size() + nameContain.size() + descriptionContain.size());
        suggestions1.insert(suggestions1.end(), nameStartOf.begin(), nameStartOf.end());
        suggestions1.insert(suggestions1.end(), nameContain.begin(), nameContain.end());
        suggestions1.insert(suggestions1.end(), descriptionContain.begin(), descriptionContain.end());
        Suggestions suggestions2(SuggestionsType::ID);
        size_t start = astNode->tokens.getStartIndex();
        size_t end = astNode->tokens.getEndIndex();
        std::transform(suggestions1.begin(), suggestions1.end(),
                       std::back_inserter(suggestions2.suggestions),
                       [&start, &end](const auto &item) {
                           return Suggestion(start, end, true, item);
                       });
        suggestions2.markFiltered();
        suggestions.push_back(std::move(suggestions2));
        return true;
    }

    void NodeCommand::collectStructure(const ASTNode *astNode,
                                       StructureBuilder &structure,
                                       bool isMustHave) const {
        if (HEDLEY_UNLIKELY(astNode == nullptr || (astNode->id == ASTNodeId::NODE_COMMAND_COMMAND && astNode->tokens.size() < 2))) {
            structure.append(isMustHave, u"命令");
            return;
        } else if (HEDLEY_LIKELY(astNode->id == ASTNodeId::NODE_COMMAND_COMMAND_NAME)) {
            structure.appendWhiteSpace().append(std::u16string(astNode->tokens.toString()));
        }
    }

    bool NodeCommand::collectColor(const ASTNode *astNode,
                                   ColoredString &coloredString,
                                   const Theme &theme) const {
        if (HEDLEY_LIKELY(astNode->id == ASTNodeId::NODE_COMMAND_COMMAND_NAME)) {
            coloredString.setColor(astNode->tokens, theme.colorCommand);
            return true;
        }
        return false;
    }

}// namespace CHelper::Node