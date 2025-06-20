//
// Created by Yancey on 2025-06-20.
//

#include <chelper/auto_suggestion/AutoSuggestion.h>
#include <chelper/node/NodeType.h>

#define CHELPER_CODEC_COLLECT_AUTO_SUGGESTION(v1)                                                                                                      \
    case Node::NodeTypeId::v1:                                                                                                                         \
        isDirty = AutoSuggestion<typename Node::NodeTypeDetail<Node::NodeTypeId::v1>::Type>::collectSuggestions(astNode, index, suggestions); \
        break;

namespace CHelper::AutoSuggestion {

    static std::shared_ptr<NormalId> spaceId = NormalId::make(u" ", u"空格");
    static std::shared_ptr<NormalId> doubleQuoteMask = NormalId::make(u"\"", u"双引号");
    static std::shared_ptr<NormalId> rangeSymbol = NormalId::make(u"..", u"范围");

    template<class NodeType>
    struct AutoSuggestion {
        static_assert(std::is_base_of_v<Node::NodeBase, NodeType>, "NodeType must be derived from NodeBase");
        static bool collectSuggestions(const ASTNode &astNode, size_t index, Suggestions &suggestions) {
            return false;
        }
    };

    template<>
    struct AutoSuggestion<Node::NodeBlock> {
        static bool collectSuggestions(const ASTNode &astNode, size_t index, Suggestions &suggestions) {
            if (HEDLEY_LIKELY(astNode.id == ASTNodeId::NODE_BLOCK_BLOCK_AND_BLOCK_STATE && !astNode.isError() &&
                              astNode.childNodes.size() == 1 && index == astNode.tokens.getEndIndex())) {
                suggestions.addSymbolSuggestion({index, index, false, Node::NodeBlock::nodeBlockStateLeftBracket->normalId});
            }
            return false;
        }
    };

    template<bool isJson>
    struct AutoSuggestion<Node::NodeTemplateBoolean<isJson>> {
        static bool collectSuggestions(const ASTNode &astNode, size_t index, Suggestions &suggestions) {
            const auto &node = reinterpret_cast<const Node::NodeTemplateBoolean<isJson> &>(*astNode.node);
            KMPMatcher kmpMatcher(astNode.tokens.toString().substr(0, index - astNode.tokens.getStartIndex()));
            if (HEDLEY_UNLIKELY(kmpMatcher.match(u"true") != std::u16string::npos)) {
                suggestions.addLiteralSuggestion({astNode.tokens, true,
                                                  NormalId::make(u"true", node.descriptionTrue)});
            }
            if (HEDLEY_UNLIKELY(kmpMatcher.match(u"false") != std::u16string::npos)) {
                suggestions.addLiteralSuggestion({astNode.tokens, true,
                                                  NormalId::make(u"false", node.descriptionFalse)});
            }
            return true;
        }
    };

    template<>
    struct AutoSuggestion<Node::NodeCommand> {
        static bool collectSuggestions(const ASTNode &astNode, size_t index, Suggestions &suggestions) {
            const auto &node = reinterpret_cast<const Node::NodeCommand &>(*astNode.node);
            if (HEDLEY_UNLIKELY(astNode.id != ASTNodeId::NODE_COMMAND_COMMAND_NAME)) {
                return false;
            }
            if (HEDLEY_LIKELY(index == 0 && astNode.tokens.isEmpty())) {
                suggestions.addSymbolSuggestion({0, 0, false, Node::NodeCommand::nodeCommandStart->normalId});
            }
            std::u16string_view str = astNode.tokens.toString().substr(0, index - astNode.tokens.getStartIndex());
            std::vector<std::shared_ptr<NormalId>> nameStartOf, nameContain, descriptionContain;
            for (const auto &item: *node.commands) {
                //通过名字进行搜索
                bool flag = false;
                for (const auto &item2: static_cast<Node::NodePerCommand *>(item.get())->name) {
                    size_t index1 = item2.find(str);
                    if (HEDLEY_UNLIKELY(index1 != std::u16string::npos)) {
                        if (HEDLEY_UNLIKELY(index1 == 0)) {
                            nameStartOf.push_back(NormalId::make(item2, item->description));
                        } else {
                            nameContain.push_back(NormalId::make(item2, item->description));
                        }
                        flag = true;
                    }
                }
                if (HEDLEY_UNLIKELY(flag)) {
                    continue;
                }
                //通过介绍进行搜索
                if (HEDLEY_UNLIKELY(item->description.has_value() &&
                                    item->description.value().find(str) != std::u16string::npos)) {
                    for (const auto &item2: static_cast<Node::NodePerCommand *>(item.get())->name) {
                        descriptionContain.push_back(NormalId::make(item2, item->description));
                    }
                }
            }
            auto compare = [](const auto &item1, const auto &item2) {
                return item1->name < item2->name;
            };
            std::sort(nameStartOf.begin(), nameStartOf.end(), compare);
            std::sort(nameContain.begin(), nameContain.end(), compare);
            std::sort(descriptionContain.begin(), descriptionContain.end(), compare);
            size_t start = astNode.tokens.getStartIndex();
            size_t end = astNode.tokens.getEndIndex();
            suggestions.reserveIdSuggestion(nameStartOf.size() + nameContain.size() + descriptionContain.size());
            for (const auto &item: nameStartOf) {
                suggestions.addIdSuggestion({start, end, true, item});
            }
            for (const auto &item: nameContain) {
                suggestions.addIdSuggestion({start, end, true, item});
            }
            for (const auto &item: descriptionContain) {
                suggestions.addIdSuggestion({start, end, true, item});
            }
            return true;
        }
    };

    template<>
    struct AutoSuggestion<Node::NodeCommandName> {
        static bool collectSuggestions(const ASTNode &astNode, size_t index, Suggestions &suggestions) {
            const auto &node = reinterpret_cast<const Node::NodeCommandName &>(*astNode.node);
            std::u16string_view str = astNode.tokens.toString().substr(0, index - astNode.tokens.getStartIndex());
            std::vector<std::shared_ptr<NormalId>> nameStartOf, nameContain, descriptionContain;
            for (const auto &item: *node.commands) {
                bool flag = false;
                for (const auto &item2: static_cast<Node::NodePerCommand *>(item.get())->name) {
                    //通过名字进行搜索
                    size_t index1 = item2.find(str);
                    if (HEDLEY_UNLIKELY(index1 != std::u16string::npos)) {
                        if (HEDLEY_UNLIKELY(index1 == 0)) {
                            nameStartOf.push_back(NormalId::make(item2, item->description));
                        } else {
                            nameContain.push_back(NormalId::make(item2, item->description));
                        }
                        flag = true;
                    }
                }
                if (HEDLEY_UNLIKELY(flag)) {
                    continue;
                }
                //通过介绍进行搜索
                if (HEDLEY_UNLIKELY(item->description.has_value() &&
                                    item->description.value().find(str) != std::u16string::npos)) {
                    for (const auto &item2: static_cast<Node::NodePerCommand *>(item.get())->name) {
                        descriptionContain.push_back(NormalId::make(item2, item->description));
                    }
                }
            }
            auto compare = [](const auto &item1, const auto &item2) {
                return item1->name < item2->name;
            };
            std::sort(nameStartOf.begin(), nameStartOf.end(), compare);
            std::sort(nameContain.begin(), nameContain.end(), compare);
            std::sort(descriptionContain.begin(), descriptionContain.end(), compare);
            size_t start = astNode.tokens.getStartIndex();
            size_t end = astNode.tokens.getEndIndex();
            suggestions.reserveIdSuggestion(nameStartOf.size() + nameContain.size() + descriptionContain.size());
            for (const auto &item: nameStartOf) {
                suggestions.addIdSuggestion({start, end, true, item});
            }
            for (const auto &item: nameContain) {
                suggestions.addIdSuggestion({start, end, true, item});
            }
            for (const auto &item: descriptionContain) {
                suggestions.addIdSuggestion({start, end, true, item});
            }
            return true;
        }
    };

    template<>
    struct AutoSuggestion<Node::NodeNamespaceId> {
        static bool collectSuggestions(const ASTNode &astNode, size_t index, Suggestions &suggestions) {
            const auto &node = reinterpret_cast<const Node::NodeNamespaceId &>(*astNode.node);
            KMPMatcher kmpMatcher(astNode.tokens.toString().substr(0, index - astNode.tokens.getStartIndex()));
            std::vector<std::shared_ptr<NormalId>> nameStartOf, nameContain;
            std::vector<std::shared_ptr<NormalId>> namespaceStartOf, namespaceContain;
            std::vector<std::shared_ptr<NamespaceId>> descriptionContain;
            for (const auto &item: *node.customContents) {
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
            size_t start = astNode.tokens.getStartIndex();
            size_t end = astNode.tokens.getEndIndex();
            suggestions.reserveIdSuggestion(nameStartOf.size() + nameContain.size() +
                                            namespaceStartOf.size() + namespaceContain.size() +
                                            (2 * descriptionContain.size()));
            for (const auto &item: nameStartOf) {
                suggestions.addIdSuggestion({start, end, node.getIsMustAfterSpace(), item});
            }
            for (const auto &item: nameContain) {
                suggestions.addIdSuggestion({start, end, node.getIsMustAfterSpace(), item});
            }
            for (const auto &item: namespaceStartOf) {
                suggestions.addIdSuggestion({start, end, node.getIsMustAfterSpace(), item});
            }
            for (const auto &item: namespaceContain) {
                suggestions.addIdSuggestion({start, end, node.getIsMustAfterSpace(), item});
            }
            for (const auto &item: descriptionContain) {
                suggestions.addIdSuggestion({start, end, node.getIsMustAfterSpace(), item});
            }
            for (const auto &item: descriptionContain) {
                suggestions.addIdSuggestion({start, end, node.getIsMustAfterSpace(), item->getIdWithNamespace()});
            }
            return true;
        }
    };

    template<>
    struct AutoSuggestion<Node::NodeNormalId> {
        static bool collectSuggestions(const ASTNode &astNode, size_t index, Suggestions &suggestions) {
            const auto &node = reinterpret_cast<const Node::NodeNormalId &>(*astNode.node);
            KMPMatcher kmpMatcher(astNode.tokens.toString().substr(0, index - astNode.tokens.getStartIndex()));
            std::vector<std::shared_ptr<NormalId>> nameStartOf, nameContain, descriptionContain;
            for (const auto &item: *node.customContents) {
                //通过名字进行搜索
                size_t index1 = kmpMatcher.match(item->name);
                if (HEDLEY_UNLIKELY(index1 != std::u16string::npos)) {
                    if (HEDLEY_UNLIKELY(index1 == 0)) {
                        nameStartOf.push_back(item);
                    } else {
                        nameContain.push_back(item);
                    }
                    continue;
                }
                //通过介绍进行搜索
                if (HEDLEY_UNLIKELY(
                            item->description.has_value() && kmpMatcher.match(item->description.value()) != std::u16string::npos)) {
                    descriptionContain.push_back(item);
                }
            }
            size_t start = astNode.tokens.getStartIndex();
            size_t end = astNode.tokens.getEndIndex();
            suggestions.reserveIdSuggestion(nameStartOf.size() + nameContain.size() + descriptionContain.size());
            for (const auto &item: nameStartOf) {
                suggestions.addIdSuggestion({start, end, node.getIsMustAfterSpace(), item});
            }
            for (const auto &item: nameContain) {
                suggestions.addIdSuggestion({start, end, node.getIsMustAfterSpace(), item});
            }
            for (const auto &item: descriptionContain) {
                suggestions.addIdSuggestion({start, end, node.getIsMustAfterSpace(), item});
            }
            return true;
        }
    };

    bool collectNodeRelativeFloatSuggestions(size_t index, Suggestions &suggestions, bool canUseCaretNotation) {
        suggestions.addSpaceSuggestion({index, index, false, spaceId});
        suggestions.addSymbolSuggestion({index, index, false, Node::NodeRelativeFloat::nodeRelativeNotation->normalId});
        if (HEDLEY_LIKELY(canUseCaretNotation)) {
            suggestions.addSymbolSuggestion({index, index, false, Node::NodeRelativeFloat::nodeCaretNotation->normalId});
        }
        return true;
    }

    template<>
    struct AutoSuggestion<Node::NodePosition> {
        static bool collectSuggestions(const ASTNode &astNode, size_t index, Suggestions &suggestions) {
            if (HEDLEY_LIKELY(astNode.id != ASTNodeId::NODE_POSITION_POSITIONS)) {
                return false;
            }
            size_t errorCount = 0;
            for (const auto &item: astNode.childNodes) {
                if (item.isError()) {
                    errorCount++;
                }
            }
            if (errorCount > 0) {
                return collectNodeRelativeFloatSuggestions(index, suggestions, true);
            }
            return true;
        }
    };

    template<>
    struct AutoSuggestion<Node::NodeRelativeFloat> {
        static bool collectSuggestions(const ASTNode &astNode, size_t index, Suggestions &suggestions) {
            const auto &node = reinterpret_cast<const Node::NodeRelativeFloat &>(*astNode.node);
            std::u16string_view str = astNode.tokens.toString();
            size_t startIndex = astNode.tokens.getStartIndex();
            for (size_t i = 0; i < str.length(); ++i) {
                if (HEDLEY_UNLIKELY(startIndex + i == index)) {
                    return collectNodeRelativeFloatSuggestions(index, suggestions, node.canUseCaretNotation);
                }
                if (str[i] != ' ') {
                    return true;
                }
            }
            if (HEDLEY_UNLIKELY(startIndex + str.length() == index)) {
                return collectNodeRelativeFloatSuggestions(index, suggestions, node.canUseCaretNotation);
            }
            return true;
        }
    };

    template<>
    struct AutoSuggestion<Node::NodeString> {
        static bool collectSuggestions(const ASTNode &astNode, size_t index, Suggestions &suggestions) {
            const auto &node = reinterpret_cast<const Node::NodeString &>(*astNode.node);
            if (HEDLEY_UNLIKELY(node.ignoreLater || !node.canContainSpace)) {
                return true;
            }
            std::u16string_view str = astNode.tokens.toString().substr(0, index - astNode.tokens.getStartIndex());
            if (HEDLEY_UNLIKELY(str.empty())) {
                suggestions.addSymbolSuggestion({index, index, false, doubleQuoteMask});
                return true;
            }
            if (HEDLEY_LIKELY(str[0] != '"')) {
                return true;
            }
            auto convertResult = JsonUtil::jsonString2String(std::u16string(str));
            if (HEDLEY_LIKELY(!convertResult.isComplete)) {
                suggestions.addSymbolSuggestion({index, index, false, doubleQuoteMask});
            }
            return true;
        }
    };

    template<>
    struct AutoSuggestion<Node::NodeTargetSelector> {
        static bool collectSuggestions(const ASTNode &astNode, size_t index, Suggestions &suggestions) {
            // const auto &node = reinterpret_cast<const Node::NodeTargetSelector &>(*astNode.node);
            // if (HEDLEY_UNLIKELY(astNode.tokens.isEmpty())) {
            //     TokensView tokens = {astNode.tokens.lexerResult, astNode.tokens.end, astNode.tokens.end};
            //     ASTNode newAstNode = ASTNode::simpleNode(&node, tokens);
            //     if (node.isWildcard) {
            //         nodeWildcard->collectSuggestions(astNode, index, suggestions);
            //     }
            //     nodeTargetSelectorVariable->collectSuggestions(astNode, index, suggestions);
            //     nodePlayerName->collectSuggestions(astNode, index, suggestions);
            //     return true;
            // }
            // if (HEDLEY_UNLIKELY(!astNode.isError() && astNode.id == ASTNodeId::NODE_TARGET_SELECTOR_NO_ARGUMENTS)) {
            //     TokensView tokens = {astNode.tokens.lexerResult, astNode.tokens.end, astNode.tokens.end};
            //     ASTNode newAstNode = ASTNode::simpleNode(&node, tokens);
            //     nodeLeft->collectSuggestions(&newAstNode, index, suggestions);
            // }
            return false;
        }
    };

    template<>
    struct AutoSuggestion<Node::NodeText> {
        static bool collectSuggestions(const ASTNode &astNode, size_t index, Suggestions &suggestions) {
            const auto &node = reinterpret_cast<const Node::NodeText &>(*astNode.node);
            std::u16string_view str = astNode.tokens.toString().substr(0, index - astNode.tokens.getStartIndex());
            //通过名字进行搜索
            size_t index1 = node.data->name.find(str);
            if (HEDLEY_LIKELY(index1 != std::u16string::npos)) {
                suggestions.addLiteralSuggestion({astNode.tokens, node.getIsMustAfterSpace(), node.data});
                return true;
            }
            //通过介绍进行搜索
            if (HEDLEY_LIKELY(node.data->description.has_value())) {
                size_t index2 = node.data->description.value().find(str);
                if (HEDLEY_LIKELY(index2 != std::u16string::npos)) {
                    suggestions.addLiteralSuggestion({astNode.tokens, node.getIsMustAfterSpace(), node.data});
                }
            }
            return true;
        }
    };

    template<>
    struct AutoSuggestion<Node::NodeRange> {
        static bool collectSuggestions(const ASTNode &astNode, size_t index, Suggestions &suggestions) {
            std::u16string_view str = astNode.tokens.toString();
            size_t index0 = str.find(u"..");
            if (HEDLEY_UNLIKELY(index0 != std::u16string::npos)) {
                index0 += astNode.tokens.getStartIndex();
                if (HEDLEY_LIKELY(index != index0 && index != index0 + 1 && index != index0 + 2)) {
                    return true;
                }
                suggestions.addSymbolSuggestion({index0, index0 + 2, false, rangeSymbol});
                return true;
            }
            size_t index1 = str.find('.');
            if (HEDLEY_UNLIKELY(index1 != std::u16string::npos)) {
                index1 += astNode.tokens.getStartIndex();
                if (HEDLEY_LIKELY(index != index1 && index != index1 + 1)) {
                    return true;
                }
                suggestions.addSymbolSuggestion({index0, index0 + 1, false, rangeSymbol});
                return true;
            }
            suggestions.addSymbolSuggestion({index, index, false, rangeSymbol});
            return true;
        }
    };

    template<>
    struct AutoSuggestion<Node::NodeJsonList> {
        static bool collectSuggestions(const ASTNode &astNode, size_t index, Suggestions &suggestions) {
            return astNode.id == ASTNodeId::NODE_JSON_ALL_LIST;
        }
    };

    template<>
    struct AutoSuggestion<Node::NodeJsonNull> {
        static bool collectSuggestions(const ASTNode &astNode, size_t index, Suggestions &suggestions) {
            std::u16string_view str = astNode.tokens.toString().substr(0, index - astNode.tokens.getStartIndex());
            if (HEDLEY_LIKELY(str.find(u"null") != std::u16string::npos)) {
                suggestions.addLiteralSuggestion({astNode.tokens, false, NormalId::make(u"null", u"null参数")});
            }
            return true;
        }
    };

    template<>
    struct AutoSuggestion<Node::NodeJsonString> {
        static bool collectSuggestions(const ASTNode &astNode, size_t index, Suggestions &suggestions) {
            std::u16string_view str = astNode.tokens.toString().substr(0, index - astNode.tokens.getStartIndex());
            if (HEDLEY_UNLIKELY(str.empty())) {
                suggestions.addSymbolSuggestion({index, index, false, doubleQuoteMask});
                return true;
            }
            auto convertResult = JsonUtil::jsonString2String(std::u16string(str));
            if (HEDLEY_UNLIKELY(astNode.id == ASTNodeId::NODE_STRING_INNER)) {
                size_t offset = astNode.tokens.getStartIndex() + 1;
                Suggestions suggestions1 = getSuggestions(astNode.childNodes[0], index - offset);
                suggestions.reserveIdSuggestion(suggestions1.spaceSuggestions.size() +
                                                suggestions1.symbolSuggestions.size() +
                                                suggestions1.literalSuggestions.size() +
                                                suggestions1.idSuggestions.size());
                for (auto &item: suggestions1.spaceSuggestions) {
                    item.start = convertResult.convert(item.start) + offset;
                    item.end = convertResult.convert(item.end) + offset;
                    std::u16string convertStr = JsonUtil::string2jsonString(item.content->name);
                    if (HEDLEY_UNLIKELY(convertStr.size() != str.size())) {
                        item.content = NormalId::make(convertStr, item.content->description);
                    }
                    suggestions.addSpaceSuggestion(std::move(item));
                }
                for (auto &item: suggestions1.symbolSuggestions) {
                    item.start = convertResult.convert(item.start) + offset;
                    item.end = convertResult.convert(item.end) + offset;
                    std::u16string convertStr = JsonUtil::string2jsonString(item.content->name);
                    if (HEDLEY_UNLIKELY(convertStr.size() != str.size())) {
                        item.content = NormalId::make(convertStr, item.content->description);
                    }
                    suggestions.addSymbolSuggestion(std::move(item));
                }
                for (auto &item: suggestions1.literalSuggestions) {
                    item.start = convertResult.convert(item.start) + offset;
                    item.end = convertResult.convert(item.end) + offset;
                    std::u16string convertStr = JsonUtil::string2jsonString(item.content->name);
                    if (HEDLEY_UNLIKELY(convertStr.size() != str.size())) {
                        item.content = NormalId::make(convertStr, item.content->description);
                    }
                    suggestions.addLiteralSuggestion(std::move(item));
                }
                for (auto &item: suggestions1.idSuggestions) {
                    item.start = convertResult.convert(item.start) + offset;
                    item.end = convertResult.convert(item.end) + offset;
                    std::u16string convertStr = JsonUtil::string2jsonString(item.content->name);
                    if (HEDLEY_UNLIKELY(convertStr.size() != str.size())) {
                        item.content = NormalId::make(convertStr, item.content->description);
                    }
                    suggestions.addIdSuggestion(std::move(item));
                }
                if (HEDLEY_LIKELY(astNode.hasChildNode() && !astNode.childNodes[0].isError() &&
                                  convertResult.errorReason == nullptr && !convertResult.isComplete)) {
                    suggestions.addSymbolSuggestion({index, index, false, doubleQuoteMask});
                }
            } else if (HEDLEY_LIKELY(convertResult.errorReason == nullptr && !convertResult.isComplete)) {
                suggestions.addSymbolSuggestion({index, index, false, doubleQuoteMask});
            }
            return true;
        }
    };

    template<>
    struct AutoSuggestion<Node::NodeOr> {
        static bool collectSuggestions(const ASTNode &astNode, size_t index, Suggestions &suggestions) {
            const auto &node = reinterpret_cast<const Node::NodeOr &>(*astNode.node);
            return node.noSuggestion;
        }
    };

    template<>
    struct AutoSuggestion<Node::NodeSingleSymbol> {
        static bool collectSuggestions(const ASTNode &astNode, size_t index, Suggestions &suggestions) {
            const auto &node = reinterpret_cast<const Node::NodeSingleSymbol &>(*astNode.node);
            if (HEDLEY_LIKELY(astNode.tokens.getStartIndex() != index)) {
                return true;
            }
            suggestions.addSymbolSuggestion({index, index, node.isAddSpace, node.normalId});
            return true;
        }
    };

    void collectSuggestions(const ASTNode &astNode, size_t index, Suggestions &suggestions) {
        if (HEDLEY_LIKELY(index < astNode.tokens.getStartIndex() || index > astNode.tokens.getEndIndex())) {
            return;
        }
        if (HEDLEY_UNLIKELY(astNode.id != ASTNodeId::COMPOUND && astNode.id != ASTNodeId::NEXT_NODE && !astNode.isAllSpaceError())) {
            bool isDirty;
#ifdef CHelperTest
            Profile::push("collect suggestions: {}", FORMAT_ARG(Node::NodeTypeHelper::getName(node->nodeTypeId)));
#endif
            switch (astNode.node->nodeTypeId) {
                CODEC_PASTE(CHELPER_CODEC_COLLECT_AUTO_SUGGESTION, CHELPER_NODE_TYPES)
                default:
                    HEDLEY_UNREACHABLE();
            }
#ifdef CHelperTest
            Profile::pop();
#endif
            if (HEDLEY_UNLIKELY(isDirty)) {
                return;
            }
        }
        switch (astNode.mode) {
            case ASTNodeMode::NONE:
                break;
            case ASTNodeMode::AND:
                for (const ASTNode &item: astNode.childNodes) {
                    collectSuggestions(item, index, suggestions);
                }
                break;
            case ASTNodeMode::OR:
                for (const ASTNode &item: astNode.childNodes) {
                    collectSuggestions(item, index, suggestions);
                }
                break;
        }
    }

    bool canAddSpace0(const ASTNode &astNode, size_t index) {
        if (std::any_of(astNode.errorReasons.begin(), astNode.errorReasons.end(),
                        [&index](const auto &item) {
                            return item->level == ErrorReasonLevel::REQUIRE_SPACE && item->start >= index && item->end <= index;
                        })) {
            return true;
        }
        switch (astNode.mode) {
            case ASTNodeMode::NONE:
                return false;
            case ASTNodeMode::AND:
                return !astNode.childNodes.empty() && canAddSpace0(astNode.childNodes[astNode.childNodes.size() - 1], index);
            case ASTNodeMode::OR:
                for (const auto &item: astNode.childNodes) {
                    if (HEDLEY_UNLIKELY(canAddSpace0(item, index))) {
                        return true;
                    }
                }
                return false;
            default:
                HEDLEY_UNREACHABLE();
        }
    }

    Suggestions getSuggestions(const ASTNode &astNode, size_t index) {
        std::u16string_view str = astNode.tokens.toString();
        Suggestions suggestions;
        if (HEDLEY_UNLIKELY(canAddSpace0(astNode, index))) {
            suggestions.addSpaceSuggestion({str.length(), str.length(), false, spaceId});
        }
        collectSuggestions(astNode, index, suggestions);
        return suggestions;
    }

}// namespace CHelper::AutoSuggestion
