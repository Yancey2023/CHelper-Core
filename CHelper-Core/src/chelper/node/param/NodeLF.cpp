//
// Created by Yancey on 2023/11/13.
//

#include <chelper/node/param/NodeLF.h>

namespace CHelper::Node {

    NodeLF::NodeLF(const std::optional<std::string> &id,
                   const std::optional<std::u16string> &description)
        : NodeSerializable(id, description, false) {}

    NodeTypeId::NodeTypeId NodeLF::getNodeType() const {
        return NodeTypeId::LF;
    }

    NodeWrapped *NodeLF::getInstance() {
        static std::unique_ptr<NodeLF> INSTANCE = std::make_unique<NodeLF>("LF", u"命令终止");
        static std::unique_ptr<NodeWrapped> INSTANCE_WRAPPED = std::make_unique<NodeWrapped>(INSTANCE.get());
        return INSTANCE_WRAPPED.get();
    }

}// namespace CHelper::Node