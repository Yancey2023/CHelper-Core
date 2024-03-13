//
// Created by Yancey on 2023/12/22.
//

#ifndef CHELPER_NODEOR_H
#define CHELPER_NODEOR_H

#include "../NodeBase.h"

namespace CHelper::Node {

    class NodeOr : public NodeBase {
    public:
        const std::vector<const NodeBase*> childNodes;
        const bool isAttachToEnd, isUseFirst;
        const std::string nodeId;

        NodeOr(const std::optional<std::string> &id,
               const std::optional<std::string> &description,
               std::vector<const NodeBase*> childNodes,
               bool isAttachToEnd,
               bool isUseFirst = false,
               std::string nodeId = std::string());

        ASTNode getASTNode(TokenReader &tokenReader, const CPack *cpack) const override;

        std::optional<std::string> collectDescription(const ASTNode *node, size_t index) const override;

    };

} // CHelper::Node


#endif //CHELPER_NODEOR_H
