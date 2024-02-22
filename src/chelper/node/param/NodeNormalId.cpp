//
// Created by Yancey666 on 2023/11/11.
//

#include "NodeNormalId.h"
#include "../../util/StringUtil.h"

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
                Profile::push(ColorStringBuilder()
                                      .red("linking contents to ")
                                      .build());
                Profile::push(ColorStringBuilder()
                                      .red("failed to find normal id in the cpack")
                                      .normal(" -> ")
                                      .purple(key.value())
                                      .build());
                throw Exception::NodeLoadFailed();
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

    bool NodeNormalId::collectIdError(const ASTNode *astNode,
                                      const CPack &cpack,
                                      std::vector<std::shared_ptr<ErrorReason>> &idErrorReasons) const {
        if (astNode->isError()) {
            return true;
        }
        std::string str = astNode->tokens[0].content;
        for (const auto &item: *contents) {
            if (str == item->name) {
                return true;
            }
        }
        idErrorReasons.push_back(ErrorReason::idError(astNode->tokens, "找不到ID -> " + str));
        return true;
    }

    bool NodeNormalId::collectSuggestions(const ASTNode *astNode,
                                          const CPack &cpack,
                                          std::vector<Suggestion> &suggestions) const {
        if (astNode->isError()) {
            return true;
        }
        std::string_view str = astNode->tokens[0].content;
        for (const auto &item: *contents) {
            if (StringUtil::isStartOf(item->name, str)) {
                suggestions.emplace_back(astNode->tokens, item);
            }
        }
        return true;
    }

} // CHelper::Node