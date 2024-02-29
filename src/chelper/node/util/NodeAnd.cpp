//
// Created by Yancey on 2023/12/22.
//

#include "NodeAnd.h"

namespace CHelper::Node {

    NodeAnd::NodeAnd(const std::optional<std::string> &id,
                     const std::optional<std::string> &description,
                     const std::shared_ptr<std::vector<std::shared_ptr<NodeBase>>> &childNodes)
            : NodeBase(id, description, false),
              childNodes(childNodes) {}

    ASTNode NodeAnd::getASTNode(TokenReader &tokenReader, const CPack &cpack) const {
        tokenReader.push();
        std::vector<ASTNode> childASTNodes;
        childASTNodes.reserve(childNodes->size());
        for (const auto &item: *childNodes) {
            childASTNodes.push_back(item->getASTNode(tokenReader, cpack));
        }
        return ASTNode::andNode(this, childASTNodes, tokenReader.collect());
    }

    std::optional<std::string> NodeAnd::collectDescription(const ASTNode *node, size_t index) const {
        return std::nullopt;
    }

} // CHelper::Node