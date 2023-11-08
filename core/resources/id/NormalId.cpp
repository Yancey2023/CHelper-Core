//
// Created by Yancey666 on 2023/11/7.
//

#include "NormalId.h"

CHelper::NormalId::NormalId(std::string name,
                   const std::optional<std::string> &description)
        : name(std::move(name)),
          description(description) {}