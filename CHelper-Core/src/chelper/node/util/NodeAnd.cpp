//
// Created by Yancey on 2023/12/22.
//

#include <chelper/node/util/NodeAnd.h>

namespace CHelper::Node {

    NodeAnd::NodeAnd(const std::optional<std::u16string> &id,
                     const std::optional<std::u16string> &description,
                     const std::vector<const NodeBase *> &childNodes)
        : NodeBase(id, description, false),
          childNodes(childNodes) {}

    NodeTypeId::NodeTypeId NodeAnd::getNodeType() const {
        return NodeTypeId::AND;
    }

    ASTNode NodeAnd::getASTNode(TokenReader &tokenReader, const CPack *cpack, void *private_data) const {
        tokenReader.push();
        std::vector<ASTNode> childASTNodes;
        for (int i = 0; i < childNodes.size(); ++i) {
            const auto &item = childNodes[i];
            auto node = item->getASTNode(tokenReader, cpack);
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
        }
        return ASTNode::andNode(this, std::move(childASTNodes), tokenReader.collect());
    }

    std::optional<std::u16string> NodeAnd::collectDescription(const ASTNode *node, size_t index) const {
        return std::nullopt;
    }

}// namespace CHelper::Node