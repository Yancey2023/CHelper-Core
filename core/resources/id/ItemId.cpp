//
// Created by Yancey666 on 2023/11/8.
//

#include "ItemId.h"

#include <utility>

CHelper::ItemId::ItemId(const std::optional<std::string> &nameSpace,
               std::string name,
               const std::optional<std::string> &description,
               const std::optional<int> &max,
               const std::optional<std::vector<std::string>> &descriptions)
        : nameSpace(nameSpace),
          name(std::move(name)),
          description(description),
          max(max),
          descriptions(descriptions) {}
