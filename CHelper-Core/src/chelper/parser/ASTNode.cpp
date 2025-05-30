//
// Created by Yancey on 2023/12/15.
//

#include <chelper/node/NodeBase.h>
#include <chelper/node/param/NodeLF.h>
#include <chelper/parser/ASTNode.h>
#include <chelper/parser/Suggestions.h>

namespace CHelper {

    ASTNode::ASTNode(ASTNodeMode::ASTNodeMode mode,
                     const Node::NodeBase *node,
                     std::vector<ASTNode> &&childNodes,
                     TokensView tokens,
                     const std::vector<std::shared_ptr<ErrorReason>> &errorReasons,
                     ASTNodeId::ASTNodeId id,
                     size_t whichBest)
        : mode(mode),
          node(node),
          childNodes(std::move(childNodes)),
          tokens(std::move(tokens)),
          errorReasons(errorReasons),
          id(id),
          whichBest(whichBest) {}

#ifdef CHelperTest 
    json ASTNode::toJson() const {
        json j;
        j["isError"] = isError();
        j["astNodeId"] = id;
        std::u16string astNodeModeStr;
        switch (mode) {
            case CHelper::ASTNodeMode::NONE:
                astNodeModeStr = "NONE";
                break;
            case CHelper::ASTNodeMode::AND:
                astNodeModeStr = "AND";
                break;
            case CHelper::ASTNodeMode::OR:
                astNodeModeStr = "OR";
                break;
            default:
                astNodeModeStr = "UNKNOWN";
                break;
        }
        j["astNodeMode"] = astNodeModeStr;
        j["nodeType"] = NodeTypeHelper::getName(node->getNodeType());
        j["nodeDescription"] = node->description.value_or("unknown");
        j["content"] = tokens.toString();
        std::u16string content = tokens.lexerResult->content;
        if (HEDLEY_LIKELY(isError())) {
            std::vector<json> errorReasonJsonList;
            for (const auto &item: errorReasons) {
                json errorJson;
                errorJson["content"] = content.substr(item->start, item->end - item->start);
                errorJson["reason"] = item->errorReason;
                errorJson["start"] = item->start;
                errorJson["end"] = item->end;
                errorReasonJsonList.push_back(errorJson);
            }
            j["errorReasons"] = errorReasonJsonList;
        } else {
            j["errorReasons"] = nullptr;
        }
        if (HEDLEY_LIKELY(hasChildNode())) {
            std::vector<json> childNodeJsonList;
            childNodeJsonList.reserve(childNodes.size());
            for (const auto &item: childNodes) {
                childNodeJsonList.push_back(item.toJson());
            }
            j["childNodes"] = childNodeJsonList;
        } else {
            j["childNodes"] = nullptr;
        }
        return j;
    }

    json ASTNode::toBestJson() const {
        if (HEDLEY_UNLIKELY(mode == ASTNodeMode::OR)) {
            return childNodes[whichBest].toBestJson();
        } else if (HEDLEY_UNLIKELY(id == ASTNodeId::COMPOUND && childNodes.size() == 1)) {
            return childNodes[0].toBestJson();
        }
        json j;
        j["isError"] = isError();
        j["astNodeId"] = id;
        std::u16string astNodeModeStr;
        switch (mode) {
            case CHelper::ASTNodeMode::NONE:
                astNodeModeStr = "NONE";
                break;
            case CHelper::ASTNodeMode::AND:
                astNodeModeStr = "AND";
                break;
            case CHelper::ASTNodeMode::OR:
                astNodeModeStr = "OR";
                break;
            default:
                astNodeModeStr = "UNKNOWN";
                break;
        }
        j["astNodeMode"] = astNodeModeStr;
        j["nodeType"] = NodeTypeHelper::getName(node->getNodeType());
        j["nodeDescription"] = node->description.value_or("unknown");
        j["content"] = tokens.toString();
        std::u16string content = tokens.lexerResult->content;
        if (HEDLEY_LIKELY(isError())) {
            std::vector<json> errorReasonJsonList;
            for (const auto &item: errorReasons) {
                json errorJson;
                errorJson["content"] = content.substr(item->start, item->end - item->start);
                errorJson["reason"] = item->errorReason;
                errorJson["start"] = item->start;
                errorJson["end"] = item->end;
                errorReasonJsonList.push_back(errorJson);
            }
            j["errorReasons"] = errorReasonJsonList;
        } else {
            j["errorReasons"] = nullptr;
        }
        if (HEDLEY_UNLIKELY(childNodes.empty())) {
            j["childNodes"] = nullptr;
        } else {
            std::vector<json> childNodeJsonList;
            childNodeJsonList.reserve(childNodes.size());
            for (const auto &item: childNodes) {
                childNodeJsonList.push_back(item.toBestJson());
            }
            j["childNodes"] = childNodeJsonList;
        }
        return j;
    }
#endif

    ASTNode ASTNode::simpleNode(const Node::NodeBase *node,
                                const TokensView &tokens,
                                const std::shared_ptr<ErrorReason> &errorReason,
                                const ASTNodeId::ASTNodeId &id) {
        std::vector<std::shared_ptr<ErrorReason>> errorReasons;
        if (HEDLEY_LIKELY(errorReason != nullptr)) {
            errorReasons.push_back(errorReason);
        }
        return {ASTNodeMode::NONE, node, {}, tokens, errorReasons, id};
    }

    ASTNode ASTNode::andNode(const Node::NodeBase *node,
                             std::vector<ASTNode> &&childNodes,
                             const TokensView &tokens,
                             const std::shared_ptr<ErrorReason> &errorReason,
                             const ASTNodeId::ASTNodeId &id) {
        if (HEDLEY_UNLIKELY(errorReason != nullptr)) {
            return {ASTNodeMode::AND, node, std::move(childNodes), tokens, {errorReason}, id};
        }
        for (const auto &item: childNodes) {
            if (HEDLEY_UNLIKELY(item.isError())) {
                return {ASTNodeMode::AND, node, std::move(childNodes), tokens, item.errorReasons, id};
            }
        }
        return {ASTNodeMode::AND, node, std::move(childNodes), tokens, {}, id};
    }

    ASTNode ASTNode::orNode(const Node::NodeBase *node,
                            std::vector<ASTNode> &&childNodes,
                            const TokensView *tokens,
                            const char16_t *errorReason,
                            const ASTNodeId::ASTNodeId &id) {
        // 收集错误的节点数，如果有节点没有错就设为0
        size_t errorCount = 0;
        for (const auto &item: childNodes) {
            if (HEDLEY_LIKELY(item.isError())) {
                errorCount++;
            } else {
                errorCount = 0;
                break;
            }
        }
        std::vector<std::shared_ptr<ErrorReason>> errorReasons;
        size_t whichBest = 0;
        if (HEDLEY_UNLIKELY(errorCount == 0)) {
            // 从没有错误的内容中找出最好的节点
            size_t end = 0;
            errorReasons.clear();
            for (size_t i = 0; i < childNodes.size(); ++i) {
                const ASTNode &item = childNodes[i];
                if (HEDLEY_LIKELY(item.isError())) {
                    continue;
                } else if (HEDLEY_LIKELY(end < item.tokens.end)) {
                    whichBest = i;
                    end = item.tokens.end;
                }
            }
            errorCount++;
        } else {
            // 收集错误原因，尝试找出错误节点中最好的节点
            size_t start = 0;
            for (size_t i = 0; i < childNodes.size(); ++i) {
                const ASTNode &item = childNodes[i];
                for (const auto &item2: item.errorReasons) {
                    if (HEDLEY_LIKELY(start > item2->start)) {
                        continue;
                    }
                    bool isAdd = true;
                    if (HEDLEY_LIKELY(start < item2->start)) {
                        start = item2->start;
                        whichBest = i;
                        errorReasons.clear();
                    } else {
                        for (const auto &item3: errorReasons) {
                            if (HEDLEY_UNLIKELY(*item2 == *item3)) {
                                isAdd = false;
                                break;
                            }
                        }
                    }
                    if (HEDLEY_LIKELY(isAdd)) {
                        errorReasons.push_back(item2);
                    }
                }
            }
        }
        TokensView tokens1 = tokens == nullptr ? childNodes[whichBest].tokens : *tokens;
        if (HEDLEY_UNLIKELY(errorCount > 1 && errorReason != nullptr)) {
            errorReasons = {ErrorReason::contentError(tokens1, errorReason)};
        }
        return {ASTNodeMode::OR, node, std::move(childNodes), tokens1, errorReasons, id, whichBest};
    }

    ASTNode ASTNode::orNode(const Node::NodeBase *node,
                            std::vector<ASTNode> &&childNodes,
                            const TokensView &tokens,
                            const char16_t *errorReason,
                            const ASTNodeId::ASTNodeId &id) {
        return orNode(node, std::move(childNodes), &tokens, errorReason, id);
    }

    bool ASTNode::isAllWhitespaceError() const {
        return isError() && std::all_of(errorReasons.begin(), errorReasons.end(),
                                        [](const auto &item) {
                                            return item->level == ErrorReasonLevel::REQUIRE_WHITE_SPACE;
                                        });
    }

    std::optional<std::u16string> ASTNode::collectDescription(size_t index) const {
        if (HEDLEY_UNLIKELY(index < tokens.getStartIndex() || index > tokens.getEndIndex())) {
            return std::nullopt;
        }
        if (HEDLEY_UNLIKELY(id != ASTNodeId::COMPOUND && id != ASTNodeId::NEXT_NODE && !isAllWhitespaceError())) {
            auto description = node->collectDescription(this, index);
            if (HEDLEY_UNLIKELY(description.has_value())) {
                return std::move(description);
            }
        }
        switch (mode) {
            case ASTNodeMode::NONE:
                return std::nullopt;
            case ASTNodeMode::AND:
                for (const ASTNode &astNode: childNodes) {
                    auto description = astNode.collectDescription(index);
                    if (HEDLEY_UNLIKELY(description.has_value())) {
                        return std::move(description);
                    }
                }
                return std::nullopt;
            case ASTNodeMode::OR:
                return childNodes[whichBest].collectDescription(index);
        }
        return std::nullopt;
    }

    //创建AST节点的时候只得到了结构的错误，ID的错误需要调用这个方法得到
    void ASTNode::collectIdErrors(std::vector<std::shared_ptr<ErrorReason>> &idErrorReasons) const {
        if (HEDLEY_UNLIKELY(id != ASTNodeId::COMPOUND && id != ASTNodeId::NEXT_NODE && !isAllWhitespaceError())) {
#ifdef CHelperTest 
            Profile::push(std::string("collect id errors: ")
                                  .append(utf8::utf16to8(NodeTypeHelper::getName(node->getNodeType())))
                                  .append(" ")
                                  .append(utf8::utf16to8(node->description.value_or(u""))));
#endif
            auto flag = node->collectIdError(this, idErrorReasons);
#ifdef CHelperTest 
            Profile::pop();
#endif
            if (HEDLEY_UNLIKELY(flag)) {
                return;
            }
        }
        switch (mode) {
            case ASTNodeMode::NONE:
                break;
            case ASTNodeMode::AND:
                for (const ASTNode &astNode: childNodes) {
                    astNode.collectIdErrors(idErrorReasons);
                }
                break;
            case ASTNodeMode::OR:
                childNodes[whichBest].collectIdErrors(idErrorReasons);
                break;
        }
    }

    void ASTNode::collectSuggestions(size_t index, std::vector<Suggestions> &suggestions) const {
        if (HEDLEY_LIKELY(index < tokens.getStartIndex() || index > tokens.getEndIndex())) {
            return;
        }
        if (HEDLEY_UNLIKELY(id != ASTNodeId::COMPOUND && id != ASTNodeId::NEXT_NODE && !isAllWhitespaceError())) {
#ifdef CHelperTest 
            Profile::push("collect suggestions: " + NodeTypeHelper::getName(node->getNodeType()) + " " + node->description.value_or(""));
#endif
            auto flag = node->collectSuggestions(this, index, suggestions);
#ifdef CHelperTest 
            Profile::pop();
#endif
            if (HEDLEY_UNLIKELY(flag)) {
                return;
            }
        }
        switch (mode) {
            case ASTNodeMode::NONE:
                break;
            case ASTNodeMode::AND:
                for (const ASTNode &astNode: childNodes) {
                    astNode.collectSuggestions(index, suggestions);
                }
                break;
            case ASTNodeMode::OR:
                for (const ASTNode &astNode: childNodes) {
                    astNode.collectSuggestions(index, suggestions);
                }
                break;
        }
    }

    void ASTNode::collectStructure(StructureBuilder &structure, bool isMustHave) const {
        bool isCompound = id == ASTNodeId::COMPOUND;
        bool isNext = id == ASTNodeId::NEXT_NODE;
        if (HEDLEY_UNLIKELY(!isCompound && !isNext)) {
            if (HEDLEY_UNLIKELY(node->brief.has_value())) {
                structure.append(isMustHave, node->brief.value());
                return;
            } else {
#ifdef CHelperTest 
                Profile::push("collect structure: {}", NodeTypeHelper::getName(node->getNodeType()) + " " + node->description.value_or(""));
#endif
                node->collectStructure(mode == ASTNodeMode::NONE && isAllWhitespaceError() ? nullptr : this, structure, isMustHave);
#ifdef CHelperTest 
                Profile::pop();
#endif
                if (HEDLEY_UNLIKELY(structure.isDirty)) {
                    structure.isDirty = false;
                    return;
                }
            }
        }
        switch (mode) {
            case ASTNodeMode::NONE:
                node->collectStructureWithNextNodes(structure, isMustHave);
                return;
            case ASTNodeMode::AND:
                for (const ASTNode &astNode: childNodes) {
                    astNode.collectStructure(structure, isMustHave);
                    if (HEDLEY_LIKELY(isMustHave)) {
                        for (const auto &item: astNode.node->nextNodes) {
                            if (HEDLEY_UNLIKELY(item == Node::NodeLF::getInstance())) {
                                isMustHave = false;
                            }
                        }
                    }
                }
                break;
            case ASTNodeMode::OR:
                if (HEDLEY_UNLIKELY(isNext && node->nextNodes.size() != 1 &&
                                    childNodes[whichBest].node == Node::NodeLF::getInstance())) {
                    for (const auto &item: node->nextNodes) {
                        if (HEDLEY_UNLIKELY(item != Node::NodeLF::getInstance())) {
                            item->collectStructureWithNextNodes(structure, isMustHave);
                            break;
                        }
                    }
                    return;
                }
                childNodes[whichBest].collectStructure(structure, isMustHave);
                break;
        }
        if (HEDLEY_UNLIKELY(isCompound && childNodes.size() <= 1 && !node->nextNodes.empty())) {
            node->nextNodes[0]->collectStructureWithNextNodes(structure, isMustHave);
        }
    }

    void ASTNode::collectColor(ColoredString &coloredString, const Theme &theme) const {
        bool isCompound = id == ASTNodeId::COMPOUND;
        bool isNext = id == ASTNodeId::NEXT_NODE;
        if (HEDLEY_UNLIKELY(!isCompound && !isNext)) {
#ifdef CHelperTest 
            Profile::push("collect color: {}", NodeTypeHelper::getName(node->getNodeType()) + " " + node->description.value_or(""));
#endif
            bool isDirty = node->collectColor(this, coloredString, theme);
#ifdef CHelperTest 
            Profile::pop();
#endif
            if (HEDLEY_UNLIKELY(isDirty)) {
                return;
            }
        }
        switch (mode) {
            case ASTNodeMode::NONE:
                return;
            case ASTNodeMode::AND:
                for (const ASTNode &astNode: childNodes) {
                    astNode.collectColor(coloredString, theme);
                }
                break;
            case ASTNodeMode::OR:
                childNodes[whichBest].collectColor(coloredString, theme);
                break;
        }
    }

    std::u16string ASTNode::getDescription(size_t index) const {
#ifdef CHelperTest 
        Profile::push("start getting description: {}", std::u16string(tokens.toString()));
#endif
        auto result = collectDescription(index).value_or(u"未知");
#ifdef CHelperTest 
        Profile::pop();
#endif
        return std::move(result);
    }

    static std::vector<std::shared_ptr<ErrorReason>> sortByLevel(const std::vector<std::shared_ptr<ErrorReason>> &input) {
        std::vector<std::shared_ptr<ErrorReason>> output;
        uint8_t i = ErrorReasonLevel::maxLevel;
        while (true) {
            for (const auto &item: input) {
                if (HEDLEY_UNLIKELY(item->level == i)) {
                    output.push_back(item);
                }
            }
            if (HEDLEY_UNLIKELY(i == 0)) {
                break;
            }
            --i;
        }
        return std::move(output);
    }

    std::vector<std::shared_ptr<ErrorReason>> ASTNode::getIdErrors() const {
        std::vector<std::shared_ptr<ErrorReason>> input;
#ifdef CHelperTest 
        Profile::push("start getting id error: " + std::u16string(tokens.toString()));
#endif
        collectIdErrors(input);
#ifdef CHelperTest 
        Profile::pop();
#endif
        return sortByLevel(input);
    }

    std::vector<std::shared_ptr<ErrorReason>> ASTNode::getErrorReasons() const {
        std::vector<std::shared_ptr<ErrorReason>> result = errorReasons;
#ifdef CHelperTest 
        Profile::push("start getting error reasons: {}", std::u16string(tokens.toString()));
#endif
        collectIdErrors(result);
#ifdef CHelperTest 
        Profile::pop();
#endif
        return sortByLevel(result);
    }

    static bool canAddWhitespace0(const ASTNode &astNode, size_t index) {
        if (std::any_of(astNode.errorReasons.begin(), astNode.errorReasons.end(),
                        [&index](const auto &item) {
                            return item->level == ErrorReasonLevel::REQUIRE_WHITE_SPACE && item->start >= index && item->end <= index;
                        })) {
            return true;
        }
        switch (astNode.mode) {
            case ASTNodeMode::NONE:
                return false;
            case ASTNodeMode::AND:
                return !astNode.childNodes.empty() && canAddWhitespace0(astNode.childNodes[astNode.childNodes.size() - 1], index);
            case ASTNodeMode::OR:
                for (const auto &item: astNode.childNodes) {
                    if (HEDLEY_UNLIKELY(canAddWhitespace0(item, index))) {
                        return true;
                    }
                }
                return false;
            default:
                HEDLEY_UNREACHABLE();
        }
    }

    std::vector<Suggestion> ASTNode::getSuggestions(size_t index) const {
        std::u16string_view str = tokens.toString();
#ifdef CHelperTest 
        Profile::push("start getting suggestions: {}", str);
#endif
        std::vector<Suggestions> suggestions;
        if (HEDLEY_UNLIKELY(canAddWhitespace0(*this, index))) {
            suggestions.push_back(Suggestions::singleWhitespaceSuggestion({str.length(), str.length(), false, whitespaceId}));
        }
        collectSuggestions(index, suggestions);
#ifdef CHelperTest 
        Profile::pop();
#endif
        return Suggestions::filter(suggestions);
    }

    std::u16string ASTNode::getStructure() const {
#ifdef CHelperTest 
        Profile::push("start getting structure: {}", tokens.toString());
#endif
        StructureBuilder structureBuilder;
        collectStructure(structureBuilder, true);
#ifdef CHelperTest 
        Profile::pop();
#endif
        std::u16string result = structureBuilder.build();
        while (HEDLEY_UNLIKELY(!result.empty() && result[result.size() - 1] == '\n')) {
            result.pop_back();
        }
        return std::move(result);
    }

    ColoredString ASTNode::getColors(const Theme &theme) const {
#ifdef CHelperTest 
        Profile::push("start getting colors: {}", std::u16string(tokens.toString()));
#endif
        ColoredString coloredString(tokens.lexerResult->content);
        collectColor(coloredString, theme);
        std::stack<char16_t> brackets;
        tokens.forEach([&brackets, &coloredString, &theme](const Token &token) {
            if (HEDLEY_LIKELY(token.type != TokenType::SYMBOL || token.content.empty())) {
                return;
            }
            char16_t ch = token.content[0];
            switch (ch) {
                case '[':
                case '{': {
                    size_t indexOfColor = brackets.size() % 3;
                    switch (indexOfColor) {
                        case 0:
                            coloredString.setColor(token.pos.index, theme.colorBrackets1);
                            break;
                        case 1:
                            coloredString.setColor(token.pos.index, theme.colorBrackets2);
                            break;
                        case 2:
                            coloredString.setColor(token.pos.index, theme.colorBrackets3);
                            break;
                        default:
                            HEDLEY_UNREACHABLE();
                    }
                    brackets.push(ch);
                } break;
                case ']':
                case '}': {
                    if (brackets.empty() || !(brackets.top() == '[' && ch == ']' || brackets.top() == '{' && ch == '}')) {
                        break;
                    }
                    size_t indexOfColor = (brackets.size() - 1) % 3;
                    switch (indexOfColor) {
                        case 0:
                            coloredString.setColor(token.pos.index, theme.colorBrackets1);
                            break;
                        case 1:
                            coloredString.setColor(token.pos.index, theme.colorBrackets2);
                            break;
                        case 2:
                            coloredString.setColor(token.pos.index, theme.colorBrackets3);
                            break;
                        default:
                            HEDLEY_UNREACHABLE();
                    }
                    brackets.pop();
                } break;
                default:
                    break;
            }
        });
#ifdef CHelperTest 
        Profile::pop();
#endif
        return coloredString;
    }

}// namespace CHelper