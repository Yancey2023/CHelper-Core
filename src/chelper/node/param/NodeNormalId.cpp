//
// Created by Yancey on 2023/11/11.
//

#include "NodeNormalId.h"

namespace CHelper::Node {

    NodeNormalId::NodeNormalId(const std::optional<std::string> &id,
                               const std::optional<std::string> &description,
                               const std::optional<std::string> &key,
                               std::shared_ptr<std::vector<std::shared_ptr<NormalId>>> &contents)
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
            contents = it->second;
        } else {
            nlohmann::json jsonArray = j.at("contents");
            contents = std::make_shared<std::vector<std::shared_ptr<NormalId>>>();
            contents->reserve(jsonArray.size());
            for (const auto &item: jsonArray) {
                contents->push_back(std::make_shared<NormalId>(item));
            }
        }
    }

    NodeType NodeNormalId::getNodeType() const {
        return NodeType::NORMAL_ID;
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
        std::string str = astNode->tokens.size() == 0 ? "" : astNode->tokens[0].content;
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
        std::string str = astNode->tokens.size() == 0 ? "" : astNode->tokens[0].content;
        for (const auto &item: *contents) {
            if (StringUtil::isStartOf(item->name, str)) {
                suggestions.emplace_back(astNode->tokens, item);
            }
        }
        return true;
    }

    void NodeNormalId::collectStructure(const ASTNode *astNode,
                                        StructureBuilder &structure,
                                        bool isMustHave) const {
        structure.append(isMustHave, description.value_or("ID"));
    }

} // CHelper::Node