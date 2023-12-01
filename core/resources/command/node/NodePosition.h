//
// Created by Yancey666 on 2023/12/2.
//

#ifndef CHELPER_NODEPOSITION_H
#define CHELPER_NODEPOSITION_H

#include "NodeBase.h"

namespace CHelper::Node {

    class NodePosition : public NodeBase{
    public:
        NodePosition(NodeType::NodeType type, const std::optional<std::string> &id,
                     const std::optional<std::string> &description);

        explicit NodePosition(const nlohmann::json &j);

        void toJson(nlohmann::json &j) const override;
    };

} // CHelper::Node

#endif //CHELPER_NODEPOSITION_H
