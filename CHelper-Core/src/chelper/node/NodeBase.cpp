//
// Created by Yancey on 2023/11/10.
//

#include <chelper/node/NodeBase.h>

namespace CHelper::Node {

    void NodeBase::init(const CPack &cpack) {
    }

    NodeSerializable::NodeSerializable(const std::optional<std::string> &id,
                                       const std::optional<std::u16string> &description,
                                       bool isMustAfterSpace)
        : id(id),
          description(description),
          isMustAfterSpace(isMustAfterSpace) {}

    bool NodeSerializable::getIsMustAfterSpace() const {
        return isMustAfterSpace.value_or(false);
    }

}// namespace CHelper::Node
