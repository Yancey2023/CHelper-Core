//
// Created by Yancey666 on 2023/11/11.
//

#ifndef CHELPER_NODECOMMANDNAME_H
#define CHELPER_NODECOMMANDNAME_H

#include "NodeBase.h"
#include "../type/NodeType.h"

namespace CHelper::Node {

    class NodeCommandName : public NodeBase {
    public:
        NodeCommandName(const std::optional<std::string> &id,
                        const std::optional<std::string> &description);

        NodeCommandName(const nlohmann::json &j,
                        const CPack &cpack);

        NODE_TYPE_H;

    };

} // CHelper::Node

CREATE_ADL_SERIALIZER(CHelper::Node::NodeCommandName);

#endif //CHELPER_NODECOMMANDNAME_H
