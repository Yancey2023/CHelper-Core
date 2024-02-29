//
// Created by Yancey on 2023/12/22.
//

#include "NodeOr.h"

#include <utility>

namespace CHelper::Node {

    NodeOr::NodeOr(const std::optional<std::string> &id,
                   const std::optional<std::string> &description,
                   const std::shared_ptr<std::vector<std::shared_ptr<NodeBase>>> &childNodes,
                   const bool isAttachToEnd,
                   std::string nodeId)
            : NodeBase(id, description, false),
              isAttachToEnd(isAttachToEnd),
              childNodes(childNodes),
              nodeId(std::move(nodeId)) {}

    ASTNode NodeOr::getASTNode(TokenReader &tokenReader, const CPack &cpack) const {
        size_t size = childNodes->size();
        std::vector<ASTNode> childASTNodes;
        childASTNodes.reserve(size);
        size_t indexes[size];
        for (int i = 0; i < size; ++i) {
            tokenReader.push();
            childASTNodes.push_back(childNodes->at(i)->getASTNode(tokenReader, cpack));
            indexes[i] = tokenReader.index;
            tokenReader.restore();
        }
        if (isAttachToEnd) {
            tokenReader.push();
            tokenReader.skipToLF();
            return ASTNode::orNode(this, childASTNodes, tokenReader.collect(), nullptr, nodeId);
        } else {
            ASTNode result = ASTNode::orNode(this, childASTNodes, nullptr, nullptr, nodeId);
            tokenReader.index = indexes[result.whichBest];
            return result;
        }
    }

    std::optional<std::string> NodeOr::collectDescription(const ASTNode *node, size_t index) const {
        return std::nullopt;
    }

} // CHelper::Node