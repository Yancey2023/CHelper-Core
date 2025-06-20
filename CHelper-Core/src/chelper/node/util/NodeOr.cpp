//
// Created by Yancey on 2023/12/22.
//

#include <chelper/node/util/NodeOr.h>

namespace CHelper::Node {

    NodeOr::NodeOr(std::vector<const NodeBase *> childNodes,
                   const bool isAttachToEnd,
                   const bool isUseFirst,
                   const bool noSuggestion,
                   const char16_t *defaultErrorReason,
                   ASTNodeId::ASTNodeId nodeId)
        : childNodes(std::move(childNodes)),
          isAttachToEnd(isAttachToEnd),
          isUseFirst(isUseFirst),
          noSuggestion(noSuggestion),
          defaultErrorReason(defaultErrorReason),
          nodeId(nodeId) {
#ifdef CHelperDebug
        for (const auto &item: this->childNodes) {
            if (HEDLEY_UNLIKELY(item == nullptr)) {
                throw std::runtime_error("null node in node or");
            }
        }
#endif
    }

    NodeTypeId::NodeTypeId NodeOr::getNodeType() const {
        return NodeTypeId::OR;
    }

}// namespace CHelper::Node