//
// Created by Yancey on 2023/11/12.
//

#include <chelper/node/NodeType.h>
#include <chelper/node/param/NodeNamespaceId.h>

namespace CHelper::Node {

    NodeNamespaceId::NodeNamespaceId(const std::optional<std::string> &id,
                                     const std::optional<std::u16string> &description,
                                     const std::optional<std::string> &key,
                                     bool ignoreError)
        : NodeSerializable(id, description, false),
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
                Profile::push("linking contents to {}", FORMAT_ARG(key.value()));
                Profile::push("failed to find namespace id in the cpack -> {}", FORMAT_ARG(key.value()));
                throw std::runtime_error("failed to find namespace id");
            } else {
                throw std::runtime_error("missing content");
            }
        }
    }

    NodeTypeId::NodeTypeId NodeNamespaceId::getNodeType() const {
        return NodeTypeId::NAMESPACE_ID;
    }

    ASTNode NodeNamespaceId::getASTNode(TokenReader &tokenReader, const CPack *cpack) const {
        // namespace:id
        // 字符串中已经包含冒号，因为冒号不是结束字符
        DEBUG_GET_NODE_BEGIN(this)
        auto result = tokenReader.readStringASTNode(this);
        DEBUG_GET_NODE_END(this)
        if (HEDLEY_UNLIKELY(result.tokens.isEmpty())) {
            TokensView tokens = result.tokens;
            return ASTNode::andNode(this, {std::move(result)}, tokens, ErrorReason::incomplete(tokens, u"命令不完整"));
        }
        if (HEDLEY_UNLIKELY(!ignoreError.value_or(false))) {
            TokensView tokens = result.tokens;
            std::u16string_view str = tokens.toString();
            XXH64_hash_t strHash = XXH3_64bits(str.data(), str.size() * sizeof(decltype(str)::value_type));
            if (HEDLEY_UNLIKELY(std::all_of(customContents->begin(), customContents->end(), [&strHash](const auto &item) {
                    return !item->fastMatch(strHash) && !item->getIdWithNamespace()->fastMatch(strHash);
                }))) {
                return ASTNode::andNode(this, {std::move(result)}, tokens, ErrorReason::incomplete(tokens, fmt::format(u"找不到含义 -> {}", str)));
            }
        }
        return result;
    }

    bool NodeNamespaceId::collectIdError(const ASTNode *astNode,
                                         std::vector<std::shared_ptr<ErrorReason>> &idErrorReasons) const {
        if (HEDLEY_UNLIKELY(astNode->isError())) {
            return true;
        }
        std::u16string_view str = astNode->tokens.toString();
        XXH64_hash_t strHash = XXH3_64bits(str.data(), str.size() * sizeof(decltype(str)::value_type));
        if (HEDLEY_UNLIKELY(std::all_of(customContents->begin(), customContents->end(), [&strHash](const auto &item) {
                return !item->fastMatch(strHash) && !item->getIdWithNamespace()->fastMatch(strHash);
            }))) {
            idErrorReasons.push_back(ErrorReason::idError(astNode->tokens, fmt::format(u"找不到ID -> {}", str)));
        }
        return true;
    }

    bool NodeNamespaceId::collectSuggestions(const ASTNode *astNode,
                                             size_t index,
                                             Suggestions &suggestions) const {
        KMPMatcher kmpMatcher(astNode->tokens.toString().substr(0, index - astNode->tokens.getStartIndex()));
        std::vector<std::shared_ptr<NormalId>> nameStartOf, nameContain;
        std::vector<std::shared_ptr<NormalId>> namespaceStartOf, namespaceContain;
        std::vector<std::shared_ptr<NamespaceId>> descriptionContain;
        for (const auto &item: *customContents) {
            //通过名字进行搜索
            size_t index1 = kmpMatcher.match(item->getIdWithNamespace()->name);
            if (HEDLEY_UNLIKELY(index1 != std::u16string::npos)) {
                //带有命名空间
                if (HEDLEY_UNLIKELY(index1 == 0)) {
                    namespaceStartOf.push_back(item->getIdWithNamespace());
                } else {
                    namespaceContain.push_back(item->getIdWithNamespace());
                }
                //省略minecraft命名空间
                constexpr size_t defaultNamespaceSize = std::size(u"minecraft");
                constexpr size_t defaultNamespacePrefixSize = std::size(u"minecraft:");
                if (HEDLEY_LIKELY(!item->idNamespace.has_value() || (item->idNamespace.value().size() == defaultNamespaceSize && item->idNamespace.value() == u"minecraft"))) {
                    if (HEDLEY_UNLIKELY(index1 == defaultNamespacePrefixSize)) {
                        nameStartOf.push_back(item);
                    } else if (HEDLEY_LIKELY(index1 > defaultNamespacePrefixSize)) {
                        nameContain.push_back(item);
                    } else {
                        size_t index2 = kmpMatcher.match(item->name);
                        if (HEDLEY_UNLIKELY(index2 == 0)) {
                            nameStartOf.push_back(item);
                        } else if (HEDLEY_UNLIKELY(index2 != std::u16string::npos)) {
                            nameContain.push_back(item);
                        }
                    }
                }
                continue;
            }
            //通过介绍进行搜索
            if (HEDLEY_UNLIKELY(
                        item->description.has_value() && kmpMatcher.match(item->description.value()) != std::u16string::npos)) {
                descriptionContain.push_back(item);
            }
        }
        size_t start = astNode->tokens.getStartIndex();
        size_t end = astNode->tokens.getEndIndex();
        suggestions.reserveIdSuggestion(nameStartOf.size() + nameContain.size() +
                                        namespaceStartOf.size() + namespaceContain.size() +
                                        2 * descriptionContain.size());
        for (const auto &item: nameStartOf) {
            suggestions.addIdSuggestion({start, end, getIsMustAfterSpace(), item});
        }
        for (const auto &item: nameContain) {
            suggestions.addIdSuggestion({start, end, getIsMustAfterSpace(), item});
        }
        for (const auto &item: namespaceStartOf) {
            suggestions.addIdSuggestion({start, end, getIsMustAfterSpace(), item});
        }
        for (const auto &item: namespaceContain) {
            suggestions.addIdSuggestion({start, end, getIsMustAfterSpace(), item});
        }
        for (const auto &item: descriptionContain) {
            suggestions.addIdSuggestion({start, end, getIsMustAfterSpace(), item});
        }
        for (const auto &item: descriptionContain) {
            suggestions.addIdSuggestion({start, end, getIsMustAfterSpace(), item->getIdWithNamespace()});
        }
        return true;
    }

    void NodeNamespaceId::collectStructure(const ASTNode *astNode,
                                           StructureBuilder &structure,
                                           bool isMustHave) const {
        structure.append(isMustHave, description.value_or(u"ID"));
    }

    bool NodeNamespaceId::collectSyntax(const ASTNode *astNode,
                                        SyntaxResult &syntaxResult) const {
        syntaxResult.update(astNode->tokens, SyntaxTokenType::ID);
        return true;
    }

}// namespace CHelper::Node