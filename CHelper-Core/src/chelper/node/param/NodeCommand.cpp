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

}// namespace CHelper::Node