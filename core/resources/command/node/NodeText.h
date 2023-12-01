//
// Created by Yancey666 on 2023/12/2.
//

#ifndef CHELPER_NODETEXT_H
#define CHELPER_NODETEXT_H

#include "NodeBase.h"
#include "../../id/NamespaceId.h"

namespace CHelper::Node {

    class NodeText : public NodeBase {
        NormalId data;

    public:
        NodeText(NodeType::NodeType type,
                 const std::optional<std::string> &id,
                 const std::optional<std::string> &description,
                 NormalId data);

        explicit NodeText(const nlohmann::json &j);

        void toJson(nlohmann::json &j) const override;
    };
} // CHelper::Node

#endif //CHELPER_NODETEXT_H
