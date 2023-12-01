//
// Created by Yancey666 on 2023/11/12.
//

#include "NodeNamespaceId.h"
#include "../../../util/Exception.h"

namespace CHelper::Node {

    NodeNamespaceId::NodeNamespaceId(const std::optional<std::string> &id,
                                     const std::optional<std::string> &description,
                                     const std::optional<std::string> &key,
                                     const std::vector<NamespaceId> *contents)
            : NodeBase(NodeType::NAMESPACE_ID, id, description),
              key(key),
              contents(contents) {}

    NodeNamespaceId::NodeNamespaceId(const nlohmann::json &j,
                                     const CPack &cpack)
            : NodeBase(NodeType::NAMESPACE_ID, j),
              key(FROM_JSON_OPTIONAL(j, key, std::string)){
        if (key.has_value()) {
            contents = &cpack.namespaceIds.find(key.value())->second;
            if (contents == nullptr) {
                throw Exception::NodeLoadFailed(Color::RED + "linking contents to " +
                                                Color::PURPLE + key.value() +
                                                Color::NORMAL,
                                                Color::RED + "failed to find namespace id in the cpack" +
                                                Color::NORMAL + " -> " +
                                                Color::PURPLE + key.value() +
                                                Color::NORMAL
                );
            }
        } else {
            nlohmann::json jsonArray = j.at("contents");
            auto* contents0 = new std::vector<NamespaceId>();
            contents0->reserve(jsonArray.size());
            for (const auto &item: jsonArray) {
                contents0->push_back(item);
            }
            contents = contents0;
        }
    }

    void NodeNamespaceId::toJson(nlohmann::json &j) const {
        NodeBase::toJson(j);
        j.push_back({"type", NodeType::STR_NAMESPACE_ID});
        TO_JSON_OPTIONAL(j, key)
        if(!key.has_value()){
            TO_JSON(j, *contents);
        }
    }
}
// CHelper::Node