//
// Created by Yancey on 2023/11/12.
//

#include "NodeNamespaceId.h"
#include "NodeString.h"

namespace CHelper::Node {

    NODE_TYPE("NAMESPACE_ID", NodeNamespaceId)

    NodeNamespaceId::NodeNamespaceId(const std::optional<std::string> &id,
                                     const std::optional<std::string> &description,
                                     const std::optional<std::string> &key,
                                     const std::vector<std::shared_ptr<NamespaceId>> *contents)
            : NodeBase(id, description),
              key(key),
              contents(contents) {}

    NodeNamespaceId::NodeNamespaceId(const nlohmann::json &j,
                                     const CPack &cpack)
            : NodeBase(j, cpack),
              key(FROM_JSON_OPTIONAL(j, key, std::string)) {
        if (key.has_value()) {
            auto it = cpack.namespaceIds.find(key.value());
            if (it == cpack.namespaceIds.end()) {
                Profile::push(ColorStringBuilder()
                                      .red("linking contents to ")
                                      .build());
                Profile::push(ColorStringBuilder()
                                      .red("failed to find namespace id in the cpack")
                                      .normal(" -> ")
                                      .purple(key.value())
                                      .build());
                throw Exception::NodeLoadFailed();
            }
            contents = &cpack.namespaceIds.find(key.value())->second;
        } else {
            nlohmann::json jsonArray = j.at("contents");
            auto *contents0 = new std::vector<std::shared_ptr<NamespaceId>>();
            contents0->reserve(jsonArray.size());
            for (const auto &item: jsonArray) {
                contents0->push_back(std::make_shared<NamespaceId>(item));
            }
            contents = contents0;
        }
    }

    NodeNamespaceId::~NodeNamespaceId() {
        if (!key.has_value()) {
            delete contents;
        }
    }

    void NodeNamespaceId::toJson(nlohmann::json &j) const {
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

    ASTNode NodeNamespaceId::getASTNode(TokenReader &tokenReader, const CPack &cpack) const {
        // 格式
        // namespace:id
        // id

        //字符串中可以包含冒号
        return getStringASTNode(tokenReader);
    }

    bool NodeNamespaceId::collectIdError(const ASTNode *astNode,
                                         const CPack &cpack,
                                         std::vector<std::shared_ptr<ErrorReason>> &idErrorReasons) const {
        if (astNode->isError()) {
            return true;
        }
        std::string str = astNode->tokens.size() == 0 ? "" : astNode->tokens[0].content;
        std::string_view nameSpace = "minecraft";
        std::string_view value = str;
        size_t index = str.find(':');
        if (index != std::string::npos) {
            nameSpace = value.substr(0, index);
            value = value.substr(index + 1);
        }
        for (const auto &item: *contents) {
            if (value == item->name && nameSpace == item->nameSpace.value_or("minecraft")) {
                return true;
            }
        }
        idErrorReasons.push_back(ErrorReason::idError(astNode->tokens, "找不到ID -> " + str));
        return true;
    }

    bool NodeNamespaceId::collectSuggestions(const ASTNode *astNode,
                                             const CPack &cpack,
                                             std::vector<Suggestion> &suggestions) const {
        if (astNode->isError()) {
            return true;
        }
        std::string str = astNode->tokens.size() == 0 ? "" : astNode->tokens[0].content;
        //省略minecraft命名空间
        for (const auto &item: *contents) {
            if ((!item->nameSpace.has_value() || item->nameSpace.value() == "minecraft") &&
                StringUtil::isStartOf(item->name, str)) {
                suggestions.emplace_back(astNode->tokens, item);
            }
        }
        //带有命名空间
        for (const auto &item: *contents) {
            if (StringUtil::isStartOf(item->idWithNamespace->name, str)) {
                suggestions.emplace_back(astNode->tokens, item->idWithNamespace);
            }
        }
        return true;
    }

    void NodeNamespaceId::collectStructure(const ASTNode *astNode,
                                           StructureBuilder &structure,
                                           bool isMustHave) const {
        structure.append(isMustHave, description.value_or("ID"));
    }

}
// CHelper::Node