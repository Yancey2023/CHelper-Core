//
// Created by Yancey666 on 2023/11/11.
//

#ifndef CHELPER_NODEFLOAT_H
#define CHELPER_NODEFLOAT_H

#include "NodeBase.h"

namespace CHelper::Node {

    class NodeFloat : public NodeBase {
    public:

        std::optional<int> min, max;

        NodeFloat(const std::optional<std::string> &id,
                  const std::optional<std::string> &description,
                  const std::optional<float> &min,
                  const std::optional<float> &max);

        NodeFloat(const nlohmann::json &j,
                  const CPack &cpack);

        NODE_TYPE_H;

        void toJson(nlohmann::json &j) const override;
    };

} // CHelper::Node

CREATE_ADL_SERIALIZER(CHelper::Node::NodeFloat);

#endif //CHELPER_NODEFLOAT_H
