//
// Created by Yancey on 2023/12/22.
//

#ifndef CHELPER_NODEAND_H
#define CHELPER_NODEAND_H

#include "../NodeBase.h"

namespace CHelper::Node {

    class NodeAnd : public NodeBase {
    public:
        std::vector<const NodeBase*> childNodes;

        NodeAnd(const std::optional<std::string> &id,
                const std::optional<std::string> &description,
                const std::vector<const NodeBase*> &childNodes);

        ASTNode getASTNode(TokenReader &tokenReader, const CPack *cpack) const override;

        std::optional<std::string> collectDescription(const ASTNode *node, size_t index) const override;

    };

} // CHelper::Node

#endif //CHELPER_NODEAND_H
