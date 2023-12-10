//
// Created by Yancey666 on 2023/11/8.
//

#include "ItemId.h"

namespace CHelper {

    ItemId::ItemId(const std::optional<std::string> &nameSpace,
                   const std::string &name,
                   const std::optional<std::string> &description,
                   const std::optional<int> &max,
                   const std::optional<std::vector<std::string>> &descriptions)
            : NamespaceId(nameSpace, name, description),
              max(max),
              descriptions(descriptions) {}

    ItemId::ItemId(const nlohmann::json &j)
            : NamespaceId(j),
              max(FROM_JSON_OPTIONAL(j, max, int)),
              descriptions(FROM_JSON_OPTIONAL(j, descriptions, std::vector<std::string>)) {}

    void ItemId::toJson(nlohmann::json &j) const {
        NamespaceId::toJson(j);
        FROM_JSON_OPTIONAL(j, max, int);
        FROM_JSON_OPTIONAL(j, descriptions, std::vector<std::string>);
    }

} // CHelper