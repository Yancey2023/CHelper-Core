//
// Created by Yancey666 on 2023/11/11.
//

#ifndef CHELPER_NODEBOOLEAN_H
#define CHELPER_NODEBOOLEAN_H

#include "NodeBase.h"

namespace CHelper::Node {

    class NodeBoolean : public NodeBase {
    public:
        std::optional<std::string> descriptionTrue, descriptionFalse;

        NodeBoolean(const std::optional<std::string> &id,
                    const std::optional<std::string> &description,
                    const std::optional<std::string> &descriptionTrue,
                    const std::optional<std::string> &descriptionFalse);

        explicit NodeBoolean(const nlohmann::json &j);

        void toJson(nlohmann::json &j) const override;
    };

} // CHelper::Node

CREATE_ADL_SERIALIZER(CHelper::Node::NodeBoolean);

#endif //CHELPER_NODEBOOLEAN_H
