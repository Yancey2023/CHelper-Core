//
// Created by Yancey on 2023/12/10.
//

#include <chelper/node/param/NodeString.h>

namespace CHelper::Node {

    NodeString::NodeString(const std::optional<std::string> &id,
                           const std::optional<std::u16string> &description,
                           bool allowMissingString,
                           const bool canContainSpace,
                           const bool ignoreLater)
        : NodeSerializable(id, description, false),
          allowMissingString(allowMissingString),
          canContainSpace(canContainSpace),
          ignoreLater(ignoreLater) {}

    NodeTypeId::NodeTypeId NodeString::getNodeType() const {
        return NodeTypeId::STRING;
    }

}// namespace CHelper::Node