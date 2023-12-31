//
// Created by Yancey666 on 2023/12/1.
//

#ifndef CHELPER_NODETARGETSELECTOR_H
#define CHELPER_NODETARGETSELECTOR_H

#include "pch.h"
#include "NodeBase.h"

namespace CHelper::Node {

    class NodeTargetSelector : public NodeBase {
    public:
        const bool isMustPlayer, isMustNPC, isOnlyOne;

        NodeTargetSelector(const std::optional<std::string> &id,
                           const std::optional<std::string> &description,
                           bool isMustPlayer,
                           bool isMustNpc,
                           bool isOnlyOne);

        explicit NodeTargetSelector(const nlohmann::json &j,
                                    const CPack &cpack);

        NODE_TYPE_H;

        void toJson(nlohmann::json &j) const override;

    };

} // CHelper::Node

CREATE_ADL_SERIALIZER(CHelper::Node::NodeTargetSelector);

#endif //CHELPER_NODETARGETSELECTOR_H
