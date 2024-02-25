//
// Created by Yancey on 2023/12/22.
//

#include "NodeEntry.h"

namespace CHelper::Node {

    NodeEntry::NodeEntry(const std::optional<std::string> &id,
                         const std::optional<std::string> &description,
                         std::shared_ptr<NodeBase> &nodeKey,
                         std::shared_ptr<NodeBase> &nodeSeparator,
                         std::shared_ptr<NodeBase> &nodeValue)
            : NodeBase(id, description, false),
              nodeKey(nodeKey),
              nodeSeparator(nodeSeparator),
              nodeValue(nodeValue) {}

    ASTNode NodeEntry::getASTNode(TokenReader &tokenReader) const {
        tokenReader.push();
        std::vector<ASTNode> childNodes = {
                nodeKey->getASTNode(tokenReader),
                nodeSeparator->getASTNode(tokenReader),
                nodeValue->getASTNode(tokenReader)
        };
        return ASTNode::andNode(this, childNodes, tokenReader.collect());
    }

    std::optional<std::string> NodeEntry::collectDescription(const ASTNode *node, size_t index) const {
        return std::nullopt;
    }

} // CHelper::Node