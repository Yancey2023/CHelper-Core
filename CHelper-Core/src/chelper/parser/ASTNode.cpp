//
// Created by Yancey on 2023/12/15.
//

#include <chelper/node/CommandNode.h>
#include <chelper/parser/ASTNode.h>

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

    bool ASTNode::isAllSpaceError() const {
        return isError() && std::all_of(errorReasons.begin(), errorReasons.end(),
                                        [](const auto &item) {
                                            return item->level == ErrorReasonLevel::REQUIRE_SPACE;
                                        });
    }

    [[nodiscard]] const ASTNode &ASTNode::getBestNode() const {
#ifdef CHelperDebug
        if (mode != ASTNodeMode::OR) {
            throw std::runtime_error("invalid mode");
        }
#endif
        return childNodes[whichBest];
    }

}// namespace CHelper