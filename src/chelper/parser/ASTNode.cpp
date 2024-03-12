//
// Created by Yancey on 2023/12/15.
//

#include "ASTNode.h"
#include "../node/NodeBase.h"
#include "../node/param/NodeLF.h"
#include "../util/TokenUtil.h"

namespace CHelper {

    ASTNode::ASTNode(ASTNodeMode::ASTNodeMode mode,
                     const Node::NodeBase *node,
                     const std::vector<ASTNode> &childNodes,
                     const VectorView <Token> &tokens,
                     const std::vector<std::shared_ptr<ErrorReason>> &errorReasons,
                     std::string id,
                     size_t whichBest)
            : mode(mode),
              node(node),
              childNodes(childNodes),
              tokens(tokens),
              errorReasons(errorReasons),
              id(std::move(id)),
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
                                const std::string &id) {
        std::vector<std::shared_ptr<ErrorReason>> errorReasons;
        if (errorReason != nullptr) {
            errorReasons.push_back(errorReason);
        }
        return {ASTNodeMode::NONE, node, {}, tokens, errorReasons, id};
    }

    ASTNode ASTNode::andNode(const Node::NodeBase *node,
                             const std::vector<ASTNode> &childNodes,
                             const VectorView <Token> &tokens,
                             const std::shared_ptr<ErrorReason> &errorReason,
                             const std::string &id) {
        if (errorReason != nullptr) {
            return {ASTNodeMode::AND, node, childNodes, tokens, {errorReason}, id};
        }
        for (const auto &item: childNodes) {
            if (item.isError()) {
                return {ASTNodeMode::AND, node, childNodes, tokens, item.errorReasons, id};
            }
        }
        return {ASTNodeMode::AND, node, childNodes, tokens, {}, id};
    }

    ASTNode ASTNode::orNode(const Node::NodeBase *node,
                            const std::vector<ASTNode> &childNodes,
                            const VectorView <Token> *tokens,
                            const std::shared_ptr<ErrorReason> &errorReason,
                            const std::string &id) {
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
                bool isAdd = true;
                if (start < item2->start) {
                    start = item2->start;
                    whichBest = i;
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
        if (errorReason != nullptr) {
            errorReasons = {errorReason};
        }
        return {ASTNodeMode::OR, node, childNodes,
                tokens == nullptr ? childNodes[whichBest].tokens : *tokens,
                errorReasons, id, whichBest};
    }

    ASTNode ASTNode::orNode(const Node::NodeBase *node,
                            const std::vector<ASTNode> &childNodes,
                            const VectorView <Token> &tokens,
                            const std::shared_ptr<ErrorReason> &errorReason,
                            const std::string &id) {
        return orNode(node, childNodes, &tokens, errorReason, id);
    }

    bool ASTNode::isError() const {
        return !errorReasons.empty();
    }

    bool ASTNode::hasChildNode() const {
        return !childNodes.empty();
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
                return description;
            }
        }
        switch (mode) {
            case ASTNodeMode::NONE:
                return std::nullopt;
            case ASTNodeMode::AND:
                for (const ASTNode &astNode: childNodes) {
                    auto description = astNode.collectDescription(index);
                    if (description.has_value()) {
                        return description;
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

    void ASTNode::collectSuggestions(size_t index, std::vector<Suggestion> &suggestions) const {
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
            node->collectStructure(isAllWhitespaceError() ? nullptr : this, structure, isMustHave);
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
        return result;
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
        return output;
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
        return output;
    }

    std::vector<Suggestion> ASTNode::getSuggestions(size_t index) const {
        Profile::push("start getting suggestions: " + TokenUtil::toString(tokens));
        std::vector<Suggestion> input, output;
        collectSuggestions(index, input);
        Profile::pop();
        for (const auto &item: input) {
            bool flag = true;
            for (const auto &item2: output) {
                if (item2.equal(item)) {
                    flag = false;
                    break;
                }
            }
            if (flag) {
                output.push_back(item);
            }
        }
        return output;
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
        return result;
    }

    std::string ASTNode::getColors() const {
        //TODO 命令语法高亮显示，获取颜色
        Profile::push("start getting colors: " + TokenUtil::toString(tokens));
        auto result = node->getNodeType()->nodeName;
        Profile::pop();
        return result;
    }

} // CHelper