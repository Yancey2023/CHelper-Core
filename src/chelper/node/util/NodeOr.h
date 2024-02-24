//
// Created by Yancey on 2023/12/22.
//

#ifndef CHELPER_NODEOR_H
#define CHELPER_NODEOR_H

#include "../NodeBase.h"

namespace CHelper::Node {

    class NodeOr : public NodeBase {
    public:
        const std::shared_ptr<std::vector<std::shared_ptr<NodeBase>>> childNodes;
        const bool isAttachToEnd;

        NodeOr(const std::optional<std::string> &id,
               const std::optional<std::string> &description,
               const std::shared_ptr<std::vector<std::shared_ptr<NodeBase>>> &childNodes,
               bool isAttachToEnd);

        ASTNode getASTNode(TokenReader &tokenReader, const CPack &cpack) const override;

    };

} // CHelper::Node


#endif //CHELPER_NODEOR_H
