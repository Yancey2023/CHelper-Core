//
// Created by Yancey on 2023/12/22.
//

#include "NodeAnd.h"

namespace CHelper::Node {

    NodeAnd::NodeAnd(const std::optional<std::string> &id,
                     const std::optional<std::string> &description,
                     const std::vector<const NodeBase *> &childNodes)
        : NodeBase(id, description, false),
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
                    i == 0 || childNodes[i - 1]->isAfterWhitespace() || item->isAfterWhitespace());
            bool isError = node.isError();
            childASTNodes.push_back(std::move(node));
            if (HEDLEY_UNLIKELY(isError)) {
                break;
            }
        }
        return ASTNode::andNode(this, std::move(childASTNodes), tokenReader.collect());
    }

    std::optional<std::string> NodeAnd::collectDescription(const ASTNode *node, size_t index) const {
        return std::nullopt;
    }

}// namespace CHelper::Node