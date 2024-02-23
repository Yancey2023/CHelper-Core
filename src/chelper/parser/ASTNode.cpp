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
                     int whichBest)
            : mode(mode),
              node(node),
              childNodes(childNodes),
              tokens(tokens),
              errorReasons(errorReasons),
              id(std::move(id)),
              whichBest(whichBest) {}

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
                            const VectorView <Token> &tokens,
                            const std::shared_ptr<ErrorReason> &errorReason,
                            const std::string &id) {
        bool isError = true;
        int whichBest = 0;
        size_t start = 0;
        std::vector<std::shared_ptr<ErrorReason>> errorReasons;
        for (int i = 0; i < childNodes.size(); ++i) {
            const ASTNode &item = childNodes[i];
            if (!item.isError()) {
                if (isError) {
                    start = 0;
                }
                isError = false;
                whichBest = i;
                errorReasons.clear();
            }
            if (!isError) {
                continue;
            }
            for (const auto &item2: item.errorReasons) {
                if (start > item2->tokens.start) {
                    continue;
                }
                bool isAdd = true;
                if (start < item2->tokens.start) {
                    start = item2->tokens.start;
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
        if (errorReason != nullptr) {
            errorReasons = {errorReason};
        }
        return {ASTNodeMode::OR, node, childNodes, tokens, errorReasons, id, whichBest};
    }

    ASTNode ASTNode::orNode(const Node::NodeBase *node,
                            const std::vector<ASTNode> &childNodes,
                            const std::shared_ptr<ErrorReason> &errorReason,
                            const std::string &id) {
        bool isError = true;
        int whichBest = 0;
        size_t start = 0;
        std::vector<std::shared_ptr<ErrorReason>> errorReasons;
        for (int i = 0; i < childNodes.size(); ++i) {
            const ASTNode &item = childNodes[i];
            if (!item.isError()) {
                if (isError) {
                    start = 0;
                }
                isError = false;
                whichBest = i;
                errorReasons.clear();
            }
            if (!isError) {
                continue;
            }
            for (const auto &item2: item.errorReasons) {
                if (start > item2->tokens.start) {
                    continue;
                }
                bool isAdd = true;
                if (start < item2->tokens.start) {
                    start = item2->tokens.start;
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
        if (errorReason != nullptr) {
            errorReasons = {errorReason};
        }
        return {ASTNodeMode::OR, node, childNodes, childNodes[whichBest].tokens, errorReasons, id, whichBest};
    }

    bool ASTNode::isError() const {
        return !errorReasons.empty();
    }

    bool ASTNode::hasChildNode() const {
        return !childNodes.empty();
    }

    std::optional<std::string> ASTNode::collectDescription(size_t index) const {
        if (index < TokenUtil::getStartIndex(tokens) || index > TokenUtil::getEndIndex(tokens)) {
            return std::nullopt;
        }
        if (id != "compound") {
            auto description = node->getDescription(this, index);
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
    void ASTNode::collectIdErrors(const CPack &cpack, std::vector<std::shared_ptr<ErrorReason>> &idErrorReasons) const {
        if (id != "compound") {
#if CHelperDebug == true
            Profile::push("collect id errors: " + node->getNodeType().nodeName + " " + node->description.value_or(""));
#endif
            auto flag = node->collectIdError(this, cpack, idErrorReasons);
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
                    astNode.collectIdErrors(cpack, idErrorReasons);
                }
                break;
            case ASTNodeMode::OR:
                childNodes[whichBest].collectIdErrors(cpack, idErrorReasons);
                break;
        }
    }

    void ASTNode::collectSuggestions(const CPack &cpack, std::vector<Suggestion> &suggestions, size_t index) const {
        if (index < TokenUtil::getStartIndex(tokens) || index > TokenUtil::getEndIndex(tokens)) {
            return;
        }
        if (id != "compound") {
#if CHelperDebug == true
            Profile::push("collect suggestions: " + node->getNodeType().nodeName
                          + " " + node->description.value_or(""));
#endif
            auto flag = node->collectSuggestions(this, cpack, suggestions);
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
                    astNode.collectSuggestions(cpack, suggestions, index);
                }
                break;
            case ASTNodeMode::OR:
                for (const ASTNode &astNode: childNodes) {
                    astNode.collectSuggestions(cpack, suggestions, index);
                }
                break;
        }
    }

    void ASTNode::collectStructure(StructureBuilder &structure, bool isMustHave) const {
        if (id != "compound") {
#if CHelperDebug == true
            Profile::push("collect structure: " + node->getNodeType().nodeName + " " + node->description.value_or(""));
#endif
            node->collectStructure(this, structure, isMustHave);
#if CHelperDebug == true
            Profile::pop();
#endif
            if (structure.isDirty()) {
                return;
            }
        }
        switch (mode) {
            case ASTNodeMode::NONE:
                structure.appendUnknownIfNotDirty(isMustHave);
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
    }

    std::string ASTNode::getDescription(size_t index) const {
        Profile::push("start getting description: " + TokenUtil::toString(tokens));
        auto result = collectDescription(index).value_or("未知");
        Profile::pop();
        return result;
    }

    std::vector<std::shared_ptr<ErrorReason>> ASTNode::getErrorReasons(const CPack &cpack) const {
        //TODO 根据错误等级调整错误的位置
        //因为大多数情况下优先先显示ID错误，所以先添加ID错误
        Profile::push("start getting error reasons: " + TokenUtil::toString(tokens));
        std::vector<std::shared_ptr<ErrorReason>> result;
        collectIdErrors(cpack, result);
        for (const auto &item: errorReasons) {
            result.push_back(item);
        }
        Profile::pop();
        return result;
    }

    std::vector<Suggestion> ASTNode::getSuggestions(const CPack &cpack, size_t index) const {
        Profile::push("start getting suggestions: " + TokenUtil::toString(tokens));
        std::vector<Suggestion> result;
        collectSuggestions(cpack, result, index);
        Profile::pop();
        return result;
    }

    std::string ASTNode::getStructure() const {
        Profile::push("start getting structure: " + TokenUtil::toString(tokens));
        StructureBuilder structureBuilder;
        collectStructure(structureBuilder, true);
        Profile::pop();
        return structureBuilder.build();
    }

    std::string ASTNode::getColors() const {
        //TODO getColors()
        Profile::push("start getting colors: " + TokenUtil::toString(tokens));
        auto result = node->getNodeType().nodeName;
        Profile::pop();
        return result;
    }

    std::ostream &operator<<(std::ostream &os, const CHelper::ASTNode &astNode) {
        os << R"({"isError": )"
           << (astNode.isError() ? "true" : "false")
           << R"(, "mode": )";
        switch (astNode.mode) {
            case CHelper::ASTNodeMode::NONE:
                os << R"("NONE")";
                break;
            case CHelper::ASTNodeMode::AND:
                os << R"("AND")";
                break;
            case CHelper::ASTNodeMode::OR:
                os << R"("OR")";
                break;
            default:
                os << R"("UNKNOWN")";
                break;
        }
        os << R"(, "type": )"
           << "\"" << astNode.node->getNodeType().nodeName << "\""
           << R"(, "description": )"
           << "\"" << astNode.node->description.value_or("unknown") << "\""
           << R"(, "content": ")"
           << CHelper::TokenUtil::toString(astNode.tokens)
           << R"(", "errorReasons": )";
        if (astNode.isError()) {
            os << "[";
            bool isFirst = true;
            for (const auto &item: astNode.errorReasons) {
                if (item->errorReason.find("指令名字不匹配") != std::string::npos) {
                    continue;
                }
                if (isFirst) {
                    isFirst = false;
                } else {
                    os << ", ";
                }
                os << R"({"content": ")"
                   << CHelper::TokenUtil::toString(item->tokens)
                   << R"(", "reason": ")"
                   << item->errorReason
                   << R"("})";
            }
            os << "]";
        } else {
            os << "null";
        }
        os << R"(, "childNodes": )";
        if (astNode.hasChildNode()) {
            os << "[";
            bool isFirst = true;
            for (const CHelper::ASTNode &item: astNode.childNodes) {
                if (astNode.id == "command" && item.childNodes.size() < 2) {
                    continue;
                }
                if (isFirst) {
                    isFirst = false;
                } else {
                    os << ", ";
                }
                os << item;
            }
            os << "]";
        } else {
            os << "null";
        }
        return os << "}";
    }

} // CHelper