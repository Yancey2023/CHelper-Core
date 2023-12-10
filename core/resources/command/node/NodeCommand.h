//
// Created by Yancey666 on 2023/11/11.
//

#ifndef CHELPER_COMMAND_H
#define CHELPER_NODECOMMAND_H

#include "NodeBase.h"

namespace CHelper::Node {

    class NodeCommand : public NodeBase {

    public:
        NodeCommand(const std::optional<std::string> &id,
                    const std::optional<std::string> &description);

        NodeCommand(const nlohmann::json &j,
                    const CPack &cpack);

        NODE_TYPE_H;

    };

} // CHelper::Node

CREATE_ADL_SERIALIZER(CHelper::Node::NodeCommand);

#endif //CHELPER_COMMAND_H
