//
// Created by Yancey on 2023/12/22.
//

#include "NodeAnd.h"

namespace CHelper::Node {

    NODE_TYPE("AND", NodeAnd)

    NodeAnd::NodeAnd(const std::optional<std::string> &id,
                     const std::optional<std::string> &description,
                     const std::vector<std::shared_ptr<NodeBase>> &childNodes)
            : NodeBase(id, description),
              childNodes(childNodes) {}

    NodeAnd::NodeAnd(const nlohmann::json &j,
                     const CPack &cpack) :
            NodeBase(j, cpack) {
        throw Exception::CantCreateInstance("CHelper::Node::NodeAnd");
    }

    ASTNode NodeAnd::getASTNode(TokenReader &tokenReader, const CPack &cpack) const {
        tokenReader.push();
        std::vector<ASTNode> childASTNodes;
        childASTNodes.reserve(childNodes.size());
        for (const auto &item: childNodes) {
            childASTNodes.push_back(item->getASTNode(tokenReader, cpack));
        }
        return ASTNode::andNode(this, childASTNodes, tokenReader.collect());
    }

} // CHelper::Node