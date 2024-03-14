//
// Created by Yancey on 2023/12/22.
//

#include "NodeOr.h"

#include <utility>

namespace CHelper::Node {

    NodeOr::NodeOr(const std::optional<std::string> &id,
                   const std::optional<std::string> &description,
                   std::vector<const NodeBase *> childNodes,
                   const bool isAttachToEnd,
                   const bool isUseFirst,
                   std::string nodeId)
            : NodeBase(id, description, false),
              isAttachToEnd(isAttachToEnd),
              isUseFirst(isUseFirst),
              childNodes(std::move(childNodes)),
              nodeId(std::move(nodeId)) {}

    ASTNode NodeOr::getASTNode(TokenReader &tokenReader, const CPack *cpack) const {
        std::vector<ASTNode> childASTNodes;
        std::vector<size_t> indexes;
        if (!isUseFirst) {
            childASTNodes.reserve(childNodes.size());
            indexes.reserve(childNodes.size());
        }
        for (const auto &item: childNodes) {
            tokenReader.push();
            ASTNode node = item->getASTNode(tokenReader, cpack);
            bool isError = node.isError();
            childASTNodes.push_back(std::move(node));
            indexes.push_back(tokenReader.index);
            tokenReader.restore();
            if (isUseFirst && !isError) {
                break;
            }
        }
        if (isAttachToEnd) {
            tokenReader.push();
            tokenReader.skipToLF();
            return ASTNode::orNode(this, std::move(childASTNodes), tokenReader.collect(), nullptr, nodeId);
        } else {
            ASTNode result = ASTNode::orNode(this, std::move(childASTNodes), nullptr, nullptr, nodeId);
            tokenReader.index = indexes[result.whichBest];
            return std::move(result);
        }
    }

    std::optional<std::string> NodeOr::collectDescription(const ASTNode *node, size_t index) const {
        return std::nullopt;
    }

} // CHelper::Node