//
// Created by Yancey on 2023/12/22.
//

#include "NodeEntry.h"

namespace CHelper::Node {

    NODE_TYPE("ENTRY", NodeEntry)

    NodeEntry::NodeEntry(const std::optional<std::string> &id,
                         const std::optional<std::string> &description,
                         std::shared_ptr<NodeBase> &nodeKey,
                         std::shared_ptr<NodeBase> &nodeSeparator,
                         std::shared_ptr<NodeBase> &nodeValue)
            : NodeBase(id, description),
              nodeKey(nodeKey),
              nodeSeparator(nodeSeparator),
              nodeValue(nodeValue) {}

    NodeEntry::NodeEntry(const nlohmann::json &j,
                         const CPack &cpack)
            : NodeBase(j, cpack) {
        throw Exception::CantCreateInstance("CHelper::Node::NodeEntry");
    }

    ASTNode NodeEntry::getASTNode(TokenReader &tokenReader, const CPack &cpack) const {
        tokenReader.push();
        std::vector<ASTNode> childNodes = {
                nodeKey->getASTNode(tokenReader, cpack),
                nodeSeparator->getASTNode(tokenReader, cpack),
                nodeValue->getASTNode(tokenReader, cpack)
        };
        return ASTNode::andNode(this, childNodes, tokenReader.collect());
    }

} // CHelper::Node