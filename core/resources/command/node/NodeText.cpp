//
// Created by Yancey666 on 2023/12/2.
//

#include "NodeText.h"

namespace CHelper::Node {

    NODE_TYPE(TEXT, NodeText);

    CHelper::Node::NodeText::NodeText(const std::optional<std::string> &id,
                                      const std::optional<std::string> &description,
                                      CHelper::NormalId data)
            : NodeBase(id, description),
              data(std::move(data)) {}

    CHelper::Node::NodeText::NodeText(const nlohmann::json &j,
                                      const CPack &cpack)
            : NodeBase(j, cpack),
              data(FROM_JSON(j, data, NormalId)) {}

    void CHelper::Node::NodeText::toJson(nlohmann::json &j) const {
        NodeBase::toJson(j);
        TO_JSON(j, data);
    }

} // CHelper::Node