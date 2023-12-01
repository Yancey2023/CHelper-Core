//
// Created by Yancey666 on 2023/11/11.
//

#ifndef CHELPER_NODECOMMANDNAME_H
#define CHELPER_NODECOMMANDNAME_H

#include "NodeBase.h"

namespace CHelper::Node {

    class NodeCommandName : public NodeBase {
    public:
        NodeCommandName(const std::optional<std::string> &id,
                        const std::optional<std::string> &description);

        explicit NodeCommandName(const nlohmann::json &j);

        void toJson(nlohmann::json &j) const override;

    };

} // CHelper::Node

CREATE_ADL_SERIALIZER(CHelper::Node::NodeCommandName);

#endif //CHELPER_NODECOMMANDNAME_H
