//
// Created by Yancey666 on 2023/12/1.
//

#include "NodeTargetSelector.h"

namespace CHelper::Node {

    NODE_TYPE(TARGET_SELECTOR, NodeTargetSelector);

    NodeTargetSelector::NodeTargetSelector(const std::optional<std::string> &id,
                                           const std::optional<std::string> &description,
                                           const bool isMustPlayer,
                                           const bool isMustNpc,
                                           const bool isOnlyOne)
            : NodeBase(id, description),
              isMustPlayer(isMustPlayer),
              isMustNPC(isMustNpc),
              isOnlyOne(isOnlyOne) {}

    NodeTargetSelector::NodeTargetSelector(const nlohmann::json &j,
                                           const CPack &cpack)
            : NodeBase(j, cpack),
              isMustPlayer(FROM_JSON(j, isMustPlayer, bool)),
              isMustNPC(FROM_JSON(j, isMustNPC, bool)),
              isOnlyOne(FROM_JSON(j, isOnlyOne, bool)) {}

    void NodeTargetSelector::toJson(nlohmann::json &j) const {
        NodeBase::toJson(j);
        TO_JSON(j, isMustPlayer);
        TO_JSON(j, isMustNPC);
        TO_JSON(j, isOnlyOne);
    }

} // CHelper::Node