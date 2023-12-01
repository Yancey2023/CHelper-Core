//
// Created by Yancey666 on 2023/11/11.
//

#ifndef CHELPER_NODECOMMAND_H
#define CHELPER_NODECOMMAND_H

#include "NodeBase.h"

namespace CHelper::Node{

    class NodeCommand : public NodeBase {

    public:
        NodeCommand(const std::optional<std::string> &id,
                    const std::optional<std::string> &description);

        explicit NodeCommand(const nlohmann::json &j);

        void toJson(nlohmann::json &j) const override;

    };

} // CHelper::Node

CREATE_ADL_SERIALIZER(CHelper::Node::NodeCommand);

#endif //CHELPER_NODECOMMAND_H
