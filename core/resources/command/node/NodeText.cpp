//
// Created by Yancey666 on 2023/12/2.
//

#include "NodeText.h"

#include <utility>

CHelper::Node::NodeText::NodeText(CHelper::Node::NodeType::NodeType type,
                                  const std::optional<std::string> &id,
                                  const std::optional<std::string> &description,
                                  CHelper::NormalId data)
        : NodeBase(type, id, description),
          data(std::move(data)) {}

CHelper::Node::NodeText::NodeText(const nlohmann::json &j)
        : NodeBase(NodeType::TEXT, j),
          data(FROM_JSON(j, data, NormalId)) {}

void CHelper::Node::NodeText::toJson(nlohmann::json &j) const {
    NodeBase::toJson(j);
    j.push_back({NodeType::STR_TEXT});
    TO_JSON(j, data);
}
