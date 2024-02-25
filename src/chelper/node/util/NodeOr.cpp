//
// Created by Yancey on 2023/12/22.
//

#include "NodeOr.h"

namespace CHelper::Node {

    NodeOr::NodeOr(const std::optional<std::string> &id,
                   const std::optional<std::string> &description,
                   const std::shared_ptr<std::vector<std::shared_ptr<NodeBase>>> &childNodes,
                   const bool isAttachToEnd)
            : NodeBase(id, description, false),
              isAttachToEnd(isAttachToEnd),
              childNodes(childNodes) {}

    ASTNode NodeOr::getASTNode(TokenReader &tokenReader) const {
        std::vector<ASTNode> childASTNodes;
        childASTNodes.reserve(childNodes->size());
        for (const auto &item: *childNodes) {
            tokenReader.push();
            childASTNodes.push_back(item->getASTNode(tokenReader));
            tokenReader.restore();
        }
        if (!isAttachToEnd) {
            return ASTNode::orNode(this, childASTNodes);
        }
        tokenReader.push();
        tokenReader.skipToLF();
        return ASTNode::orNode(this, childASTNodes, tokenReader.collect());
    }

    std::optional<std::string> NodeOr::collectDescription(const ASTNode *node, size_t index) const {
        return std::nullopt;
    }

} // CHelper::Node