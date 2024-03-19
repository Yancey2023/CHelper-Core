//
// Created by Yancey on 2023/12/15.
//

#include "ASTNode.h"
#include "../node/NodeBase.h"
#include "../node/param/NodeLF.h"
#include "../util/TokenUtil.h"
#include "Suggestions.h"

namespace CHelper {

    ASTNode::ASTNode(ASTNodeMode::ASTNodeMode mode,
                     const Node::NodeBase *node,
                     std::vector<ASTNode> &&childNodes,
                     const VectorView <Token> &tokens,
                     const std::vector<std::shared_ptr<ErrorReason>> &errorReasons,
                     std::string id,
                     bool canAddWhitespace,
                     size_t whichBest)
            : mode(mode),
              node(node),
              childNodes(std::move(childNodes)),
              tokens(tokens),
              errorReasons(errorReasons),
              id(std::move(id)),
              canAddWhitespace(canAddWhitespace),
              whichBest(whichBest) {}

    nlohmann::json ASTNode::toJson() const {
        nlohmann::json j;
        j["isError"] = isError();
        if (HEDLEY_LIKELY(id.empty())) {
            j["astNodeId"] = nullptr;
        } else {
            j["astNodeId"] = id;
        }
        std::string astNodeModeStr;
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
        j["nodeType"] = node->getNodeType()->nodeName;
        j["nodeDescription"] = node->description.value_or("unknown");
        j["content"] = TokenUtil::toString(tokens);
        std::string content = TokenUtil::toString({tokens.vector, 0, tokens.vector->size()});
        if (HEDLEY_LIKELY(isError())) {
            std::vector<nlohmann::json> errorReasonJsonList;
            for (const auto &item: errorReasons) {
                nlohmann::json errorJson;
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
            std::vector<nlohmann::json> childNodeJsonList;
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

    nlohmann::json ASTNode::toBestJson() const {
        if (HEDLEY_UNLIKELY(mode == ASTNodeMode::OR)) {
            return childNodes[whichBest].toBestJson();
        } else if (HEDLEY_UNLIKELY(id == "compound" && childNodes.size() == 1)) {
            return childNodes[0].toBestJson();
        }
        nlohmann::json j;
        j["isError"] = isError();
        if (HEDLEY_LIKELY(id.empty())) {
            j["astNodeId"] = nullptr;
        } else {
            j["astNodeId"] = id;
        }
        std::string astNodeModeStr;
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
        j["nodeType"] = node->getNodeType()->nodeName;
        j["nodeDescription"] = node->description.value_or("unknown");
        j["content"] = TokenUtil::toString(tokens);
        std::string content = TokenUtil::toString({tokens.vector, 0, tokens.vector->size()});
        if (HEDLEY_LIKELY(isError())) {
            std::vector<nlohmann::json> errorReasonJsonList;
            for (const auto &item: errorReasons) {
                nlohmann::json errorJson;
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
            std::vector<nlohmann::json> childNodeJsonList;
            childNodeJsonList.reserve(childNodes.size());
            for (const auto &item: childNodes) {
                childNodeJsonList.push_back(item.toBestJson());
            }
            j["childNodes"] = childNodeJsonList;
        }
        return j;
    }

    ASTNode ASTNode::simpleNode(const Node::NodeBase *node,
                                const VectorView <Token> &tokens,
                                const std::shared_ptr<ErrorReason> &errorReason,
                                const std::string &id,
                                bool canAddWhitespace) {
        std::vector<std::shared_ptr<ErrorReason>> errorReasons;
        if (HEDLEY_LIKELY(errorReason != nullptr)) {
            errorReasons.push_back(errorReason);
        }
        return {ASTNodeMode::NONE, node, {}, tokens, errorReasons, id, canAddWhitespace};
    }

    ASTNode ASTNode::andNode(const Node::NodeBase *node,
                             std::vector<ASTNode> &&childNodes,
                             const VectorView <Token> &tokens,
                             const std::shared_ptr<ErrorReason> &errorReason,
                             const std::string &id,
                             bool canAddWhitespace) {
        if (HEDLEY_LIKELY(canAddWhitespace)) {
            for (const auto &item: childNodes) {
                if (HEDLEY_LIKELY(item.tokens.size() != 0)) {
                    canAddWhitespace = item.canAddWhitespace;
                }
            }
        }
        if (HEDLEY_UNLIKELY(errorReason != nullptr)) {
            return {ASTNodeMode::AND, node, std::move(childNodes), tokens, {errorReason}, id, canAddWhitespace};
        }
        for (const auto &item: childNodes) {
            if (HEDLEY_UNLIKELY(item.isError())) {
                return {ASTNodeMode::AND, node, std::move(childNodes), tokens, item.errorReasons, id, canAddWhitespace};
            }
        }
        return {ASTNodeMode::AND, node, std::move(childNodes), tokens, {}, id, canAddWhitespace};
    }

    ASTNode ASTNode::orNode(const Node::NodeBase *node,
                            std::vector<ASTNode> &&childNodes,
                            const VectorView <Token> *tokens,
                            const char *errorReason,
                            const std::string &id,
                            bool canAddWhitespace) {
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
        if (HEDLEY_LIKELY(canAddWhitespace)) {
            canAddWhitespace = std::all_of(childNodes.begin(), childNodes.end(),
                                           [](const auto &item) {
                                               return item.canAddWhitespace;
                                           });
        }
        VectorView <Token> tokens1 = tokens == nullptr ? childNodes[whichBest].tokens : *tokens;
        if (HEDLEY_UNLIKELY(errorCount > 1 && errorReason != nullptr)) {
            errorReasons = {ErrorReason::contentError(tokens1, errorReason)};
        }
        return {ASTNodeMode::OR, node, std::move(childNodes), tokens1, errorReasons, id, canAddWhitespace, whichBest};

    }

    ASTNode ASTNode::orNode(const Node::NodeBase *node,
                            std::vector<ASTNode> &&childNodes,
                            const VectorView <Token> &tokens,
                            const char *errorReason,
                            const std::string &id,
                            bool canAddWhitespace) {
        return orNode(node, std::move(childNodes), &tokens, errorReason, id, canAddWhitespace);
    }

    bool ASTNode::isAllWhitespaceError() const {
        return isError() && std::all_of(errorReasons.begin(), errorReasons.end(),
                                        [](const auto &item) {
                                            return item->level == ErrorReasonLevel::REQUIRE_WHITE_SPACE;
                                        });
    }

    std::optional<std::string> ASTNode::collectDescription(size_t index) const {
        if (HEDLEY_UNLIKELY(index < TokenUtil::getStartIndex(tokens) || index > TokenUtil::getEndIndex(tokens))) {
            return std::nullopt;
        }
        if (HEDLEY_UNLIKELY(id != "compound" && id != "nextNode" && !isAllWhitespaceError())) {
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
        if (HEDLEY_UNLIKELY(id != "compound" && id != "nextNode" && !isAllWhitespaceError())) {
#if CHelperDebug == true
            Profile::push(std::string("collect id errors: ")
                                  .append(node->getNodeType()->nodeName)
                                  .append(" ")
                                  .append(node->description.value_or("")));
#endif
            auto flag = node->collectIdError(this, idErrorReasons);
#if CHelperDebug == true
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
        if (HEDLEY_LIKELY(index < TokenUtil::getStartIndex(tokens) || index > TokenUtil::getEndIndex(tokens))) {
            return;
        }
        if (HEDLEY_UNLIKELY(id != "compound" && id != "nextNode" && !isAllWhitespaceError())) {
#if CHelperDebug == true
            Profile::push("collect suggestions: " + node->getNodeType()->nodeName
                          + " " + node->description.value_or(""));
#endif
            auto flag = node->collectSuggestions(this, index, suggestions);
#if CHelperDebug == true
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
        bool isCompound = id == "compound";
        bool isNext = id == "nextNode";
        if (HEDLEY_UNLIKELY(!isCompound && !isNext)) {
            if (HEDLEY_UNLIKELY(node->brief.has_value())) {
                structure.append(isMustHave, node->brief.value());
                return;
            } else {
#if CHelperDebug == true
                Profile::push(ColorStringBuilder()
                                      .red("collect structure: ")
                                      .purple(node->getNodeType()->nodeName + " " + node->description.value_or(""))
                                      .build());
#endif
                node->collectStructure(mode == ASTNodeMode::NONE && isAllWhitespaceError() ?
                                       nullptr : this, structure, isMustHave);
#if CHelperDebug == true
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

    std::string ASTNode::getDescription(size_t index) const {
        Profile::push("start getting description: " + TokenUtil::toString(tokens));
        auto result = collectDescription(index).value_or("未知");
        Profile::pop();
        return std::move(result);
    }

    std::vector<std::shared_ptr<ErrorReason>> ASTNode::getIdErrors() const {
        std::vector<std::shared_ptr<ErrorReason>> input, output;
        Profile::push("start getting id error: " + TokenUtil::toString(tokens));
        collectIdErrors(input);
        Profile::pop();
        for (size_t i = ErrorReasonLevel::maxLevel; i >= 0; --i) {
            for (const auto &item: input) {
                if (HEDLEY_UNLIKELY(item->level == i)) {
                    output.push_back(item);
                }
            }
        }
        return std::move(output);
    }

    std::vector<std::shared_ptr<ErrorReason>> ASTNode::getErrorReasons() const {
        std::vector<std::shared_ptr<ErrorReason>> input = errorReasons, output;
        Profile::push("start getting error reasons: " + TokenUtil::toString(tokens));
        collectIdErrors(input);
        Profile::pop();
        std::uint8_t i = ErrorReasonLevel::maxLevel;
        do {
            for (const auto &item: input) {
                if (HEDLEY_UNLIKELY(item->level == i)) {
                    output.push_back(item);
                }
            }
        } while (--i != 0);
        return std::move(output);
    }

    std::vector<Suggestion> ASTNode::getSuggestions(size_t index) const {
        Profile::push("start getting suggestions: " + TokenUtil::toString(tokens));
        std::vector<Suggestions> suggestions;
        collectSuggestions(index, suggestions);
        Profile::pop();
        return Suggestions::filter(suggestions);
    }

    std::string ASTNode::getStructure() const {
        Profile::push("start getting structure: " + TokenUtil::toString(tokens));
        StructureBuilder structureBuilder;
        collectStructure(structureBuilder, true);
        Profile::pop();
        std::string result = structureBuilder.build();
        while (HEDLEY_UNLIKELY(!result.empty() && result[result.size() - 1] == '\n')) {
            result.pop_back();
        }
        return std::move(result);
    }

    std::string ASTNode::getColors() const {
        //TODO 命令语法高亮显示，获取颜色
        Profile::push("start getting colors: " + TokenUtil::toString(tokens));
        auto result = node->getNodeType()->nodeName;
        Profile::pop();
        return result;
    }

} // CHelper