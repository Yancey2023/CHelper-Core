//
// Created by Yancey on 2023/12/22.
//

#include <chelper/node/util/NodeAnd.h>
#include <chelper/node/util/NodeWrapped.h>

namespace CHelper::Node {

    NodeAnd::NodeAnd(const std::vector<const NodeBase *> &childNodes)
        : childNodes(childNodes) {}

    NodeTypeId::NodeTypeId NodeAnd::getNodeType() const {
        return NodeTypeId::AND;
    }

    ASTNode NodeAnd::getASTNode(TokenReader &tokenReader, const CPack *cpack) const {
        tokenReader.push();
        std::vector<ASTNode> childASTNodes;
        bool isMustAfterWhitespace = false;
        for (size_t i = 0; i < childNodes.size(); ++i) {
            const auto &item = childNodes[i];
            if (item->getNodeType() == NodeTypeId::WRAPPED) {
                const NodeWrapped * nodeWrapped = reinterpret_cast<const NodeWrapped *>(item);
                ASTNode node = nodeWrapped->getASTNodeWithIsMustAfterWhitespace(tokenReader, cpack, isMustAfterWhitespace);
                bool isError = node.isError();
                childASTNodes.push_back(std::move(node));
                if (HEDLEY_UNLIKELY(isError)) {
                    break;
                }
                isMustAfterWhitespace = nodeWrapped->innerNode->getIsMustAfterWhitespace();
            } else {
                ASTNode node = item->getASTNode(tokenReader, cpack);
                bool isError = node.isError();
                childASTNodes.push_back(std::move(node));
                if (HEDLEY_UNLIKELY(isError)) {
                    break;
                }
                if (HEDLEY_UNLIKELY(i < childNodes.size() - 1 &&
                                    tokenReader.ready() &&
                                    tokenReader.peek()->type == TokenType::WHITE_SPACE)) {
                    tokenReader.push();
                    tokenReader.skip();
                    TokensView tokens = tokenReader.collect();
                    return ASTNode::andNode(this, std::move(childASTNodes), tokenReader.collect(),
                                            ErrorReason::contentError(tokens, u"意外的空格"));
                }
                isMustAfterWhitespace = false;
            }
        }
        return ASTNode::andNode(this, std::move(childASTNodes), tokenReader.collect());
    }

    std::optional<std::u16string> NodeAnd::collectDescription(const ASTNode *node, size_t index) const {
        return std::nullopt;
    }

}// namespace CHelper::Node