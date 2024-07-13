//
// Created by Yancey on 2023/11/12.
//

#include "NodeNamespaceId.h"
#include "NodeString.h"

namespace CHelper::Node {

    NodeNamespaceId::NodeNamespaceId(const std::optional<std::string> &id,
                                     const std::optional<std::string> &description,
                                     const std::optional<std::string> &key,
                                     bool ignoreError)
        : NodeBase(id, description, false),
          key(key),
          ignoreError(ignoreError) {}

    void NodeNamespaceId::init(const CPack &cpack) {
        if (HEDLEY_LIKELY(contents.has_value())) {
            customContents = contents.value();
        } else if (HEDLEY_LIKELY(key.has_value())) {
            customContents = cpack.getNamespaceId(key.value());
        }
        if (HEDLEY_UNLIKELY(customContents == nullptr)) {
            if (HEDLEY_UNLIKELY(key.has_value())) {
                Profile::push(ColorStringBuilder()
                                      .red("linking contents to ")
                                      .purple(key.value())
                                      .build());
                Profile::push(ColorStringBuilder()
                                      .red("failed to find namespace id in the cpack")
                                      .normal(" -> ")
                                      .purple(key.value())
                                      .build());
            } else {
                Profile::push(ColorStringBuilder()
                                      .red("missing content")
                                      .build());
            }
            throw Exception::NodeLoadFailed();
        }
    }

    NodeType *NodeNamespaceId::getNodeType() const {
        return NodeType::NAMESPACE_ID.get();
    }

    ASTNode NodeNamespaceId::getASTNode(TokenReader &tokenReader, const CPack *cpack) const {
        // namespace:id
        // 字符串中已经包含冒号，因为冒号不是结束字符
        DEBUG_GET_NODE_BEGIN(this)
        auto result = tokenReader.readStringASTNode(this);
        DEBUG_GET_NODE_END(this)
        if (HEDLEY_UNLIKELY(result.tokens.isEmpty())) {
            TokensView tokens = result.tokens;
            return ASTNode::andNode(this, {std::move(result)}, tokens, ErrorReason::incomplete(tokens, "命令不完整"));
        }
        if (HEDLEY_UNLIKELY(!ignoreError.value_or(false))) {
            TokensView tokens = result.tokens;
            std::string str = tokens.toString();
            size_t strHash = std::hash<std::string>{}(str);
            if (HEDLEY_UNLIKELY(std::all_of(customContents->begin(), customContents->end(), [&strHash](const auto &item) {
                    return !item->fastMatch(strHash) && !item->getIdWithNamespace()->fastMatch(strHash);
                }))) {
                return ASTNode::andNode(this, {std::move(result)}, tokens, ErrorReason::incomplete(tokens, "找不到含义 -> " + std::move(str)));
            }
        }
        return result;
    }

    bool NodeNamespaceId::collectIdError(const ASTNode *astNode,
                                         std::vector<std::shared_ptr<ErrorReason>> &idErrorReasons) const {
        if (HEDLEY_UNLIKELY(astNode->isError())) {
            return true;
        }
        std::string str = astNode->tokens.toString();
        size_t strHash = std::hash<std::string>{}(str);
        if (HEDLEY_UNLIKELY(std::all_of(customContents->begin(), customContents->end(), [&strHash](const auto &item) {
                return !item->fastMatch(strHash) && !item->getIdWithNamespace()->fastMatch(strHash);
            }))) {
            idErrorReasons.push_back(ErrorReason::idError(astNode->tokens, std::string("找不到ID -> ").append(str)));
        }
        return true;
    }

    bool NodeNamespaceId::collectSuggestions(const ASTNode *astNode,
                                             size_t index,
                                             std::vector<Suggestions> &suggestions) const {
        std::string str = astNode->tokens.toString()
                                  .substr(0, index - astNode->tokens.getStartIndex());
        std::vector<std::shared_ptr<NormalId>> nameStartOf, nameContain;
        std::vector<std::shared_ptr<NormalId>> namespaceStartOf, namespaceContain;
        std::vector<std::shared_ptr<NamespaceId>> descriptionContain;
        for (const auto &item: *customContents) {
            //通过名字进行搜索
            //省略minecraft命名空间
            if (HEDLEY_LIKELY((!item->idNamespace.has_value() || item->idNamespace.value() == "minecraft"))) {
                size_t index1 = item->name.find(str);
                if (HEDLEY_UNLIKELY(index1 == 0)) {
                    nameStartOf.push_back(item);
                } else if (HEDLEY_UNLIKELY(index1 != std::string::npos)) {
                    nameContain.push_back(item);
                }
            }
            //带有命名空间
            size_t index1 = item->getIdWithNamespace()->name.find(str);
            if (HEDLEY_UNLIKELY(index1 != std::string::npos)) {
                if (HEDLEY_UNLIKELY(index1 == 0)) {
                    namespaceStartOf.push_back(item->getIdWithNamespace());
                } else {
                    namespaceContain.push_back(item->getIdWithNamespace());
                }
                continue;
            }
            //通过介绍进行搜索
            if (HEDLEY_UNLIKELY(
                        item->description.has_value() && item->description.value().find(str) != std::string::npos)) {
                descriptionContain.push_back(item);
            }
        }
        Suggestions suggestions1;
        suggestions1.suggestions.reserve(nameStartOf.size() + nameContain.size() +
                                         namespaceStartOf.size() + namespaceContain.size() +
                                         2 * descriptionContain.size());
        size_t start = astNode->tokens.getStartIndex();
        size_t end = astNode->tokens.getEndIndex();
        std::transform(nameStartOf.begin(), nameStartOf.end(),
                       std::back_inserter(suggestions1.suggestions),
                       [&start, &end, this](const auto &item) {
                           return Suggestion(start, end, isAfterWhitespace(), item);
                       });
        std::transform(nameContain.begin(), nameContain.end(),
                       std::back_inserter(suggestions1.suggestions),
                       [&start, &end, this](const auto &item) {
                           return Suggestion(start, end, isAfterWhitespace(), item);
                       });
        std::transform(namespaceStartOf.begin(), namespaceStartOf.end(),
                       std::back_inserter(suggestions1.suggestions),
                       [&start, &end, this](const auto &item) {
                           return Suggestion(start, end, isAfterWhitespace(), item);
                       });
        std::transform(namespaceContain.begin(), namespaceContain.end(),
                       std::back_inserter(suggestions1.suggestions),
                       [&start, &end, this](const auto &item) {
                           return Suggestion(start, end, isAfterWhitespace(), item);
                       });
        std::transform(descriptionContain.begin(), descriptionContain.end(),
                       std::back_inserter(suggestions1.suggestions),
                       [&start, &end, this](const auto &item) {
                           return Suggestion(start, end, isAfterWhitespace(), item);
                       });
        std::transform(descriptionContain.begin(), descriptionContain.end(),
                       std::back_inserter(suggestions1.suggestions),
                       [&start, &end, this](const auto &item) {
                           return Suggestion(start, end, isAfterWhitespace(), item->getIdWithNamespace());
                       });
        suggestions1.markFiltered();
        suggestions.push_back(std::move(suggestions1));
        return true;
    }

    void NodeNamespaceId::collectStructure(const ASTNode *astNode,
                                           StructureBuilder &structure,
                                           bool isMustHave) const {
        structure.append(isMustHave, description.value_or("ID"));
    }

    CODEC_NODE(NodeNamespaceId, key, ignoreError, contents)

}// namespace CHelper::Node