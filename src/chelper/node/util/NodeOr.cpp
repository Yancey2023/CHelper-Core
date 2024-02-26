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
        size_t size = childNodes->size();
        std::vector<ASTNode> childASTNodes;
        childASTNodes.reserve(size);
        size_t indexes[size];
        for (int i = 0; i < size; ++i) {
            tokenReader.push();
            childASTNodes.push_back(childNodes->at(i)->getASTNode(tokenReader));
            indexes[i] = tokenReader.index;
            tokenReader.restore();
        }
        if (isAttachToEnd) {
            tokenReader.push();
            tokenReader.skipToLF();
            return ASTNode::orNode(this, childASTNodes, tokenReader.collect());
        } else {
            ASTNode result = ASTNode::orNode(this, childASTNodes);
            tokenReader.index = indexes[result.whichBest];
            return result;
        }
    }

    std::optional<std::string> NodeOr::collectDescription(const ASTNode *node, size_t index) const {
        return std::nullopt;
    }

} // CHelper::Node