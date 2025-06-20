//
// Created by Yancey on 2024/2/27.
//

#include <chelper/node/param/NodeRange.h>
#include <chelper/node/param/NodeText.h>

namespace CHelper::Node {

    NodeRange::NodeRange(const std::optional<std::string> &id,
                         const std::optional<std::u16string> &description)
        : NodeSerializable(id, description, false) {}

    NodeTypeId::NodeTypeId NodeRange::getNodeType() const {
        return NodeTypeId::RANGE;
    }

}// namespace CHelper::Node