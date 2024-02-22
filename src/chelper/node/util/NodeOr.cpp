//
// Created by Yancey666 on 2023/12/22.
//

#include "NodeOr.h"

namespace CHelper::Node {

    NODE_TYPE("OR", NodeOr)

    NodeOr::NodeOr(const std::optional<std::string> &id,
                   const std::optional<std::string> &description,
                   const std::vector<std::shared_ptr<NodeBase>> &childNodes,
                   const bool isAttachToEnd)
            : NodeBase(id, description),
              isAttachToEnd(isAttachToEnd),
              childNodes(childNodes) {}

    NodeOr::NodeOr(const nlohmann::json &j,
                   const CPack &cpack) :
            NodeBase(j, cpack),
            isAttachToEnd(false) {
        throw Exception::CantCreateInstance("CHelper::Node::NodeAnd");
    }

    ASTNode NodeOr::getASTNode(TokenReader &tokenReader, const CPack &cpack) const {
        std::vector<ASTNode> childASTNodes;
        childASTNodes.reserve(childNodes.size());
        for (const auto &item: childNodes) {
            tokenReader.push();
            childASTNodes.push_back(item->getASTNode(tokenReader, cpack));
            tokenReader.restore();
        }
        //TODO NodeOr当isAttachToEnd为false的时候，应该检测成功的内容，如果有多个成功，应该以第一个为准，并给出警告
        if (!isAttachToEnd) {
            return ASTNode::orNode(this, childASTNodes);
        }
        tokenReader.push();
        tokenReader.skipToLF();
        return ASTNode::orNode(this, childASTNodes, tokenReader.collect());
    }

} // CHelper::Node