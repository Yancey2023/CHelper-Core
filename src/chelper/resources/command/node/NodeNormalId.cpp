//
// Created by Yancey666 on 2023/11/11.
//

#include "NodeNormalId.h"

namespace CHelper::Node {

    NODE_TYPE("NORMAL_ID", NodeNormalId)

    NodeNormalId::NodeNormalId(const std::optional<std::string> &id,
                               const std::optional<std::string> &description,
                               const std::optional<std::string> &key,
                               const std::vector<std::shared_ptr<NormalId>> *contents)
            : NodeBase(id, description),
              key(key),
              contents(contents) {}

    NodeNormalId::NodeNormalId(const nlohmann::json &j,
                               const CPack &cpack)
            : NodeBase(j, cpack),
              key(FROM_JSON_OPTIONAL(j, key, std::string)) {
        if (key.has_value()) {
            auto it = cpack.normalIds.find(key.value());
            if (it == cpack.normalIds.end()) {
                throw Exception::NodeLoadFailed(Color::RED + "linking contents to " +
                                                Color::PURPLE + key.value() +
                                                Color::NORMAL,
                                                Color::RED + "failed to find namespace id in the cpack" +
                                                Color::NORMAL + " -> " +
                                                Color::PURPLE + key.value() +
                                                Color::NORMAL
                );
            }
            contents = &cpack.normalIds.find(key.value())->second;
        } else {
            nlohmann::json jsonArray = j.at("contents");
            auto *contents0 = new std::vector<std::shared_ptr<NormalId>>();
            contents0->reserve(jsonArray.size());
            for (const auto &item: jsonArray) {
                contents0->push_back(std::make_shared<NormalId>(item));
            }
            contents = contents0;
        }
    }

    NodeNormalId::~NodeNormalId() {
        if (!key.has_value()) {
            delete contents;
        }
    }

    void NodeNormalId::toJson(nlohmann::json &j) const {
        NodeBase::toJson(j);
        TO_JSON_OPTIONAL(j, key)
        if (!key.has_value()) {
            nlohmann::json content;
            for (const auto &item: *contents) {
                content.push_back(*item);
            }
            j.push_back({"contents", content});
        }
    }

    ASTNode NodeNormalId::getASTNode(TokenReader &tokenReader, const CPack &cpack) const {
        return getStringASTNode(tokenReader);
    }

} // CHelper::Node