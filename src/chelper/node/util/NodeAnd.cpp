//
// Created by Yancey on 2023/12/22.
//

#include "NodeAnd.h"

namespace CHelper::Node {

    NodeAnd::NodeAnd(const std::optional<std::wstring> &id,
                     const std::optional<std::wstring> &description,
                     WhitespaceMode::WhitespaceMode whitespaceMode,
                     const std::vector<const NodeBase *> &childNodes)
        : NodeBase(id, description, false),
          whitespaceMode(whitespaceMode),
          childNodes(childNodes) {}

    NodeType *NodeAnd::getNodeType() const {
        return NodeType::AND.get();
    }

    ASTNode NodeAnd::getASTNode(TokenReader &tokenReader, const CPack *cpack) const {
        tokenReader.push();
        std::vector<ASTNode> childASTNodes;
        for (int i = 0; i < childNodes.size(); ++i) {
            const auto &item = childNodes[i];
            auto node = item->getASTNodeWithNextNode(
                    tokenReader, cpack,
                    whitespaceMode == WhitespaceMode::NORMAL &&
                            (i == 0 || childNodes[i - 1]->isAfterWhitespace() || item->isAfterWhitespace()));
            bool isError = node.isError();
            childASTNodes.push_back(std::move(node));
            if (HEDLEY_UNLIKELY(isError)) {
                break;
            }
            if (HEDLEY_UNLIKELY(whitespaceMode == WhitespaceMode::NO_WHITESPACE &&
                                i < childNodes.size() - 1 &&
                                tokenReader.ready() &&
                                tokenReader.peek()->type == TokenType::WHITE_SPACE)) {
                tokenReader.push();
                tokenReader.skip();
                TokensView tokens = tokenReader.collect();
                return ASTNode::andNode(this, std::move(childASTNodes), tokenReader.collect(),
                                        ErrorReason::contentError(tokens, L"意外的空格"));
            }
        }
        return ASTNode::andNode(this, std::move(childASTNodes), tokenReader.collect());
    }

    std::optional<std::wstring> NodeAnd::collectDescription(const ASTNode *node, size_t index) const {
        return std::nullopt;
    }

}// namespace CHelper::Node