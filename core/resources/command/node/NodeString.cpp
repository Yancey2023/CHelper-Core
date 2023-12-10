//
// Created by Yancey666 on 2023/12/10.
//

#include "NodeString.h"

namespace CHelper::Node {

    NODE_TYPE(STRING, NodeString);

    NodeString::NodeString(const std::optional<std::string> &id,
                           const std::optional<std::string> &description,
                           const bool canContainSpace,
                           const bool ignoreLater)
            : NodeBase(id, description),
              canContainSpace(canContainSpace),
              ignoreLater(ignoreLater) {}

    NodeString::NodeString(const nlohmann::json &j,
                           const CPack &cpack)
            : NodeBase(j, cpack),
              canContainSpace(FROM_JSON(j, canContainSpace, bool)),
              ignoreLater(FROM_JSON(j, ignoreLater, bool)) {}


} // CHelper::Node