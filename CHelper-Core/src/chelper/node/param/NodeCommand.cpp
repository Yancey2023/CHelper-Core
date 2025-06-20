//
// Created by Yancey on 2023/11/11.
//

#include <chelper/node/param/NodeCommand.h>
#include <chelper/node/util/NodeSingleSymbol.h>
#include <chelper/resources/CPack.h>

namespace CHelper::Node {

    std::unique_ptr<Node::NodeSingleSymbol> NodeCommand::nodeCommandStart = std::make_unique<Node::NodeSingleSymbol>(u'/', u"命令开始字符");

    NodeCommand::NodeCommand(const std::optional<std::string> &id,
                             const std::optional<std::u16string> &description,
                             std::vector<std::unique_ptr<Node::NodePerCommand>> *commands)
        : NodeSerializable(id, description, false),
          commands(commands) {}

    void NodeCommand::init(const CPack &cpack) {
        commands = cpack.commands.get();
    }

    NodeTypeId::NodeTypeId NodeCommand::getNodeType() const {
        return NodeTypeId::COMMAND;
    }

    ASTNode NodeCommand::getASTNode(TokenReader &tokenReader, const CPack *cpack) const {
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
                currentCommand = static_cast<NodePerCommand *>(item.get());
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

    void NodeCommand::collectStructure(const ASTNode *astNode,
                                       StructureBuilder &structure,
                                       bool isMustHave) const {
        if (HEDLEY_UNLIKELY(astNode == nullptr || (astNode->id == ASTNodeId::NODE_COMMAND_COMMAND && astNode->tokens.size() < 2))) {
            structure.append(isMustHave, u"命令");
        } else if (HEDLEY_LIKELY(astNode->id == ASTNodeId::NODE_COMMAND_COMMAND_NAME)) {
            structure.appendSpace().append(std::u16string(astNode->tokens.toString()));
        }
    }

    bool NodeCommand::collectSyntax(const ASTNode *astNode,
                                    SyntaxResult &syntaxResult) const {
        if (HEDLEY_LIKELY(astNode->id == ASTNodeId::NODE_COMMAND_COMMAND_NAME)) {
            syntaxResult.update(astNode->tokens, SyntaxTokenType::COMMAND);
            return true;
        }
        return false;
    }

}// namespace CHelper::Node