//
// Created by Yancey666 on 2023/12/15.
//

#include "ASTNode.h"

#include <utility>
#include "../resources/command/node/NodeBase.h"

namespace CHelper {

    ASTNode::ASTNode(ASTNodeMode::ASTNodeMode mode,
                     const Node::NodeBase *node,
                     const std::vector<ASTNode> &childNodes,
                     const VectorView<Token> &tokens,
                     const std::vector<std::shared_ptr<ErrorReason>> &errorReasons,
                     std::string id = "unknown")
            : mode(mode),
              node(node),
              childNodes(childNodes),
              tokens(tokens),
              errorReasons(errorReasons),
              id(std::move(id)) {}

    ASTNode ASTNode::simpleNode(const Node::NodeBase *node,
                                const VectorView<Token> &tokens,
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
                             const VectorView<Token> &tokens,
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
                            const VectorView<Token> &tokens,
                            const std::shared_ptr<ErrorReason> &errorReason,
                            const std::string &id) {
        if (errorReason != nullptr) {
            return {ASTNodeMode::OR, node, childNodes, tokens, {errorReason}, id};
        }
        size_t start = 0;
        std::vector<std::shared_ptr<ErrorReason>> errorReasons;
        for (const auto &item: childNodes) {
            if (!item.isError()) {
                errorReasons.clear();
                break;
            }
            if (start < item.tokens.start) {
                start = item.tokens.start;
                errorReasons.clear();
            }
            for (const auto &item2: item.errorReasons) {
                bool isAdd = true;
                for (const auto &item3: errorReasons) {
                    if (*item2 == *item3) {
                        isAdd = false;
                    }
                }
                if (isAdd) {
                    errorReasons.push_back(item2);
                }
            }
        }
        return {ASTNodeMode::OR, node, childNodes, tokens, errorReasons, id};
    }

    bool ASTNode::isError() const {
        return !errorReasons.empty();
    }

    bool ASTNode::hasChildNode() const {
        return childNodes.has_value();
    }

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
       << R"(, "content": ")";
    std::string result;
    astNode.tokens.for_each([&result](const CHelper::Token &token) {
        if (!result.empty() && token.whiteSpace) {
            result.push_back(' ');
        }
        result.append(token.content);
    });
    os << result;
    os << R"(", "errorReason": )";
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
            os << R"({"content": ")";
            std::string result1;
            item->tokens.for_each([&result1](const CHelper::Token &token) {
                if (!result1.empty() && token.whiteSpace) {
                    result1.push_back(' ');
                }
                result1.append(token.content);
            });
            os << result1;
            os << R"(", "reason": ")";
            os << item->errorReason;
            os << R"("})";
        }
        os << "]";
    } else {
        os << "null";
    }
    os << R"(, "childNodes": )";
    if (astNode.childNodes.has_value()) {
        os << "[";
        bool isFirst = true;
        for (const auto &item: astNode.childNodes.value()) {
            if (item.childNodes.value().size() < 2) {
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