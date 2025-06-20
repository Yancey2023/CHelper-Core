//
// Created by Yancey on 2023/12/2.
//

#include <chelper/node/param/NodePosition.h>
#include <chelper/node/param/NodeRelativeFloat.h>

namespace CHelper::Node {

    NodePosition::NodePosition(const std::optional<std::string> &id,
                               const std::optional<std::u16string> &description)
        : NodeSerializable(id, description, false) {}

    NodeTypeId::NodeTypeId NodePosition::getNodeType() const {
        return NodeTypeId::POSITION;
    }

}// namespace CHelper::Node