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

    ASTNode NodeAnd::getASTNode(TokenReader &tokenReader, const CPack *cpack) const {
        tokenReader.push();
        std::vector<ASTNode> childASTNodes;
        for (const auto &item: childNodes) {
            auto node = item->getASTNodeWithNextNode(tokenReader, cpack);
            bool isError = node.isError();
            childASTNodes.push_back(std::move(node));
            if (isError) {
                break;
            }
        }
        return ASTNode::andNode(this, std::move(childASTNodes), tokenReader.collect());
    }

    std::optional<std::string> NodeAnd::collectDescription(const ASTNode *node, size_t index) const {
        return std::nullopt;
    }

} // CHelper::Node