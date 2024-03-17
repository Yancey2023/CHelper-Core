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
        std::string modeStr;
        switch (mode) {
            case CHelper::ASTNodeMode::NONE:
                modeStr = "NONE";
                break;
            case CHelper::ASTNodeMode::AND:
                modeStr = "AND";
                break;
            case CHelper::ASTNodeMode::OR:
                modeStr = "OR";
                break;
            default:
                modeStr = "UNKNOWN";
                break;
        }
        j["mode"] = modeStr;
        j["type"] = node->getNodeType()->nodeName;
        j["description"] = node->description.value_or("unknown");
        j["content"] = TokenUtil::toString(tokens);
        std::string content = TokenUtil::toString({tokens.vector, 0, tokens.vector->size()});
        if (isError()) {
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
        if (hasChildNode()) {
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

    nlohmann::json ASTNode::toOptimizedJson() const {
        nlohmann::json j;
        j["isError"] = isError();
        std::string modeStr;
        switch (mode) {
            case CHelper::ASTNodeMode::NONE:
                modeStr = "NONE";
                break;
            case CHelper::ASTNodeMode::AND:
                modeStr = "AND";
                break;
            case CHelper::ASTNodeMode::OR:
                modeStr = "OR";
                break;
            default:
                modeStr = "UNKNOWN";
                break;
        }
        j["mode"] = modeStr;
        j["type"] = node->getNodeType()->nodeName;
        j["description"] = node->description.value_or("unknown");
        j["content"] = TokenUtil::toString(tokens);
        std::string content = TokenUtil::toString({tokens.vector, 0, tokens.vector->size()});
        if (isError()) {
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
        if (hasChildNode()) {
            std::vector<nlohmann::json> childNodeJsonList;
            childNodeJsonList.reserve(childNodes.size());
            for (const auto &item: childNodes) {
                if (id == "command_or" && item.childNodes.size() < 2) {
                    continue;
                }
                childNodeJsonList.push_back(item.toOptimizedJson());
            }
            j["childNodes"] = childNodeJsonList;
        } else {
            j["childNodes"] = nullptr;
        }
        return j;
    }

    nlohmann::json ASTNode::toBestJson() const {
        if (mode == ASTNodeMode::OR) {
            return childNodes[whichBest].toBestJson();
        } else if (id == "compound" && childNodes.size() == 1) {
            return childNodes[0].toBestJson();
        }
        nlohmann::json j;
        j["isError"] = isError();
        std::string modeStr;
        switch (mode) {
            case CHelper::ASTNodeMode::NONE:
                modeStr = "NONE";
                break;
            case CHelper::ASTNodeMode::AND:
                modeStr = "AND";
                break;
            case CHelper::ASTNodeMode::OR:
                modeStr = "OR";
                break;
            default:
                modeStr = "UNKNOWN";
                break;
        }
        j["mode"] = modeStr;
        j["type"] = node->getNodeType()->nodeName;
        j["description"] = node->description.value_or("unknown");
        j["content"] = TokenUtil::toString(tokens);
        std::string content = TokenUtil::toString({tokens.vector, 0, tokens.vector->size()});
        if (isError()) {
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
        if (childNodes.empty()) {
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
        if (errorReason != nullptr) {
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
        if (canAddWhitespace) {
            for (const auto &item: childNodes) {
                if (item.tokens.size() != 0) {
                    canAddWhitespace = item.canAddWhitespace;
                }
            }
        }
        if (errorReason != nullptr) {
            return {ASTNodeMode::AND, node, std::move(childNodes), tokens, {errorReason}, id, canAddWhitespace};
        }
        for (const auto &item: childNodes) {
            if (item.isError()) {
                return {ASTNodeMode::AND, node, std::move(childNodes), tokens, item.errorReasons, id, canAddWhitespace};
            }
        }
        return {ASTNodeMode::AND, node, std::move(childNodes), tokens, {}, id, canAddWhitespace};
    }

    ASTNode ASTNode::orNode(const Node::NodeBase *node,
                            std::vector<ASTNode> &&childNodes,
                            const VectorView <Token> *tokens,
                            const std::shared_ptr<ErrorReason> &errorReason,
                            const std::string &id,
                            bool canAddWhitespace) {
        bool isError = true;
        size_t whichBest = 0;
        size_t start = 0;
        std::vector<std::shared_ptr<ErrorReason>> errorReasons;
        for (int i = 0; i < childNodes.size(); ++i) {
            const ASTNode &item = childNodes[i];
            if (!item.isError()) {
                isError = false;
                break;
            }
            for (const auto &item2: item.errorReasons) {
                if (start > item2->start) {
                    continue;
                }
                bool isBetter = start < item2->start;
                if (isBetter) {
                    start = item2->start;
                    whichBest = i;
                }
                if (errorReason == nullptr) {
                    bool isAdd = true;
                    if (isBetter) {
                        errorReasons.clear();
                    } else {
                        for (const auto &item3: errorReasons) {
                            if (*item2 == *item3) {
                                isAdd = false;
                            }
                        }
                    }
                    if (isAdd) {
                        errorReasons.push_back(item2);
                    }
                }
            }
        }
        if (!isError) {
            size_t maxEnd = 0;
            errorReasons.clear();
            for (int i = 0; i < childNodes.size(); ++i) {
                const ASTNode &item = childNodes[i];
                if (item.isError()) {
                    continue;
                }
                if (maxEnd < item.tokens.size()) {
                    whichBest = i;
                    maxEnd = item.tokens.size();
                }
            }
        }
        if (canAddWhitespace) {
            canAddWhitespace = std::all_of(childNodes.begin(), childNodes.end(),
                                           [](const auto &item) {
                                               return item.canAddWhitespace;
                                           });
        }
        if (errorReason != nullptr) {
            errorReasons = {errorReason};
        }
        VectorView <Token> tokens1 = tokens == nullptr ? childNodes[whichBest].tokens : *tokens;
        return {ASTNodeMode::OR, node, std::move(childNodes), tokens1, errorReasons, id, canAddWhitespace, whichBest};

    }

    ASTNode ASTNode::orNode(const Node::NodeBase *node,
                            std::vector<ASTNode> &&childNodes,
                            const VectorView <Token> &tokens,
                            const std::shared_ptr<ErrorReason> &errorReason,
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
        if (index < TokenUtil::getStartIndex(tokens) || index > TokenUtil::getEndIndex(tokens)) {
            return std::nullopt;
        }
        if (id != "compound" && id != "nextNode" && !isAllWhitespaceError()) {
            auto description = node->collectDescription(this, index);
            if (description.has_value()) {
                return std::move(description);
            }
        }
        switch (mode) {
            case ASTNodeMode::NONE:
                return std::nullopt;
            case ASTNodeMode::AND:
                for (const ASTNode &astNode: childNodes) {
                    auto description = astNode.collectDescription(index);
                    if (description.has_value()) {
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
        if (id != "compound" && id != "nextNode" && !isAllWhitespaceError()) {
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
            if (flag) {
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
        if (index < TokenUtil::getStartIndex(tokens) || index > TokenUtil::getEndIndex(tokens)) {
            return;
        }
        if (id != "compound" && id != "nextNode" && !isAllWhitespaceError()) {
#if CHelperDebug == true
            Profile::push("collect suggestions: " + node->getNodeType()->nodeName
                          + " " + node->description.value_or(""));
#endif
            auto flag = node->collectSuggestions(this, index, suggestions);
#if CHelperDebug == true
            Profile::pop();
#endif
            if (flag) {
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
        if (!isCompound && id != "nextNode") {
#if CHelperDebug == true
            Profile::push("collect structure: " + node->getNodeType()->nodeName + " " + node->description.value_or(""));
#endif
            node->collectStructure(mode == ASTNodeMode::NONE && isAllWhitespaceError() ?
                                   nullptr : this, structure, isMustHave);
#if CHelperDebug == true
            Profile::pop();
#endif
            if (structure.isDirty) {
                structure.isDirty = false;
                return;
            }
        }
        switch (mode) {
            case ASTNodeMode::NONE:
                structure.appendUnknown(isMustHave);
                structure.isDirty = false;
                break;
            case ASTNodeMode::AND:
                for (const ASTNode &astNode: childNodes) {
                    astNode.collectStructure(structure, isMustHave);
                    if (isMustHave) {
                        for (const auto &item: astNode.node->nextNodes) {
                            if (item == Node::NodeLF::getInstance()) {
                                isMustHave = false;
                            }
                        }
                    }
                }
                break;
            case ASTNodeMode::OR:
                childNodes[whichBest].collectStructure(structure, isMustHave);
                break;
        }
        if (isCompound && childNodes.size() <= 1 && !node->nextNodes.empty()) {
            node->nextNodes[0]->collectStructureNormal(structure, isMustHave);
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
        for (int i = ErrorReasonLevel::maxLevel; i >= 0; --i) {
            for (const auto &item: input) {
                if (item->level == i) {
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
        for (int i = ErrorReasonLevel::maxLevel; i >= 0; --i) {
            for (const auto &item: input) {
                if (item->level == i) {
                    output.push_back(item);
                }
            }
        }
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
        if (!result.empty() && result[result.size() - 1] == '\n') {
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