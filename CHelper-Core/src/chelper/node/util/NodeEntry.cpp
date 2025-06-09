//
// Created by Yancey on 2023/12/22.
//

#include <chelper/node/util/NodeEntry.h>

namespace CHelper::Node {

    NodeEntry::NodeEntry(NodeBase *nodeKey,
                         NodeBase *nodeSeparator,
                         NodeBase *nodeValue)
        : nodeKey(nodeKey),
          nodeSeparator(nodeSeparator),
          nodeValue(nodeValue) {}

    NodeTypeId::NodeTypeId NodeEntry::getNodeType() const {
        return NodeTypeId::ENTRY;
    }

    ASTNode NodeEntry::getASTNode(TokenReader &tokenReader, const CPack *cpack) const {
        tokenReader.push();
        std::vector<ASTNode> childNodes;
        auto key = nodeKey->getASTNode(tokenReader, cpack);
        if (HEDLEY_UNLIKELY(key.isError())) {
            childNodes.push_back(std::move(key));
            return ASTNode::andNode(this, std::move(childNodes), tokenReader.collect());
        }
        childNodes.push_back(std::move(key));
        auto separator = nodeSeparator->getASTNode(tokenReader, cpack);
        if (HEDLEY_UNLIKELY(separator.isError())) {
            childNodes.push_back(std::move(separator));
            return ASTNode::andNode(this, std::move(childNodes), tokenReader.collect());
        }
        childNodes.push_back(std::move(separator));
        childNodes.push_back(nodeValue->getASTNode(tokenReader, cpack));
        return ASTNode::andNode(this, std::move(childNodes), tokenReader.collect());
    }

    std::optional<std::u16string> NodeEntry::collectDescription(const ASTNode *node, size_t index) const {
        return std::nullopt;
    }

}// namespace CHelper::Node