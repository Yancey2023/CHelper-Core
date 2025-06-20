//
// Created by Yancey on 2024/2/28.
//

#include <chelper/node/json/NodeJsonNull.h>

namespace CHelper::Node {

    NodeJsonNull::NodeJsonNull(const std::optional<std::string> &id,
                               const std::optional<std::u16string> &description)
        : NodeSerializable(id, description, false) {}

    NodeTypeId::NodeTypeId NodeJsonNull::getNodeType() const {
        return NodeTypeId::JSON_NULL;
    }

}// namespace CHelper::Node