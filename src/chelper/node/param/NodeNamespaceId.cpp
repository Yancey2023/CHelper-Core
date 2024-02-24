//
// Created by Yancey on 2023/11/12.
//

#include "NodeNamespaceId.h"
#include "NodeString.h"

namespace CHelper::Node {

    NodeNamespaceId::NodeNamespaceId(const std::optional<std::string> &id,
                                     const std::optional<std::string> &description,
                                     const std::optional<std::string> &key,
                                     const std::shared_ptr<std::vector<std::shared_ptr<NamespaceId>>> &contents)
            : NodeBase(id, description),
              key(key),
              contents(contents) {}

    std::shared_ptr<std::vector<std::shared_ptr<NamespaceId>>> getContentFromCPack(const nlohmann::json &j,
                                                                                   const CPack &cpack,
                                                                                   const std::optional<std::string> &key) {
        if (key.has_value()) {
            auto it = cpack.namespaceIds.find(key.value());
            if (it == cpack.namespaceIds.end()) {
                Profile::push(ColorStringBuilder()
                                      .red("linking contents to ")
                                      .purple(key.value())
                                      .build());
                Profile::push(ColorStringBuilder()
                                      .red("failed to find namespace id in the cpack")
                                      .normal(" -> ")
                                      .purple(key.value())
                                      .build());
                throw Exception::NodeLoadFailed();
            }
            return it->second;
        } else {
            nlohmann::json jsonArray = j.at("contents");
            auto contents = std::make_shared<std::vector<std::shared_ptr<NamespaceId>>>();
            contents->reserve(jsonArray.size());
            for (const auto &item: jsonArray) {
                contents->push_back(std::make_shared<NamespaceId>(item));
            }
            return contents;
        }
    }

    NodeNamespaceId::NodeNamespaceId(const nlohmann::json &j,
                                     const CPack &cpack)
            : NodeBase(j, cpack),
              key(FROM_JSON_OPTIONAL(j, key, std::string)),
              contents(getContentFromCPack(j, cpack, key)) {}

    NodeType NodeNamespaceId::getNodeType() const {
        return NodeType::NAMESPACE_ID;
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
        // namespace:id
        // 字符串中已经包含冒号，因为冒号不是结束字符
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