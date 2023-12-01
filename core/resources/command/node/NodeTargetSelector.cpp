//
// Created by Yancey666 on 2023/12/1.
//

#include "NodeTargetSelector.h"

namespace CHelper::Node {

    NodeTargetSelector::NodeTargetSelector(NodeType::NodeType type,
                                           const std::optional<std::string> &id,
                                           const std::optional<std::string> &description,
                                           const bool isMustPlayer,
                                           const bool isMustNpc,
                                           const bool isOnlyOne)
            : NodeBase(type, id, description),
              isMustPlayer(isMustPlayer),
              isMustNPC(isMustNpc),
              isOnlyOne(isOnlyOne) {}

    NodeTargetSelector::NodeTargetSelector(const nlohmann::json &j)
            : NodeBase(NodeType::TARGET_SELECTOR, j),
              isMustPlayer(FROM_JSON(j, isMustPlayer, bool)),
              isMustNPC(FROM_JSON(j, isMustNPC, bool)),
              isOnlyOne(FROM_JSON(j, isOnlyOne, bool)) {}

    void NodeTargetSelector::toJson(nlohmann::json &j) const {
        NodeBase::toJson(j);
        j.push_back({"type", NodeType::STR_TARGET_SELECTOR});
        TO_JSON(j, isMustPlayer);
        TO_JSON(j, isMustNPC);
        TO_JSON(j, isOnlyOne);
    }

} // CHelper::Node