//
// Created by Yancey on 2024/2/20.
//

#ifndef CHELPER_NODETARGETSELECTORARGUMENT_H
#define CHELPER_NODETARGETSELECTORARGUMENT_H

#include "../NodeBase.h"
#include "../param/NodeNormalId.h"
#include "../param/NodeNamespaceId.h"

namespace CHelper::Node {

    class NodeTargetSelectorArgument : public NodeBase {
    private:
        std::shared_ptr<NodeBase> nodeFamily, nodeGameMode, nodeEntities,
                nodeHasItemElement, nodeHasItemList1, nodeHasItemList2, nodeHasItem;

    public:
        NodeTargetSelectorArgument(const std::optional<std::string> &id,
                                   const std::optional<std::string> &description,
                                   const std::shared_ptr<NodeBase> &nodeItem,
                                   const std::shared_ptr<NodeBase> &nodeFamily,
                                   const std::shared_ptr<NodeBase> &nodeGameMode,
                                   const std::shared_ptr<NodeBase> &nodeSlot,
                                   const std::shared_ptr<NodeBase> &nodeEntities);

        ASTNode getASTNode(TokenReader &tokenReader, const CPack &cpack) const override;
    };

} // CHelper::Node

#endif //CHELPER_NODETARGETSELECTORARGUMENT_H
