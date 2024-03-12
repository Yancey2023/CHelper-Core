//
// Created by Yancey on 2024/2/20.
//

#ifndef CHELPER_NODETARGETSELECTORARGUMENT_H
#define CHELPER_NODETARGETSELECTORARGUMENT_H

#include "../NodeBase.h"
#include "../param/NodeNormalId.h"
#include "../param/NodeNamespaceId.h"
#include "NodeHasItemArgument.h"
#include "../util/NodeList.h"

namespace CHelper::Node {

    class NodeTargetSelectorArgument : public NodeBase {
    private:
        const NodeBase *nodeFamily, *nodeGameMode, *nodeEntities;
        const NodeHasItemArgument nodeHasItemElement;
        const NodeList nodeHasItemList1, nodeHasItemList2;
        const NodeOr nodeHasItem;

    public:
        NodeTargetSelectorArgument(const std::optional<std::string> &id,
                                   const std::optional<std::string> &description,
                                   const NodeBase *nodeItem,
                                   const NodeBase *nodeFamily,
                                   const NodeBase *nodeGameMode,
                                   const NodeBase *nodeSlot,
                                   const NodeBase *nodeEntities);

        ASTNode getASTNode(TokenReader &tokenReader, const CPack *cpack) const override;
    };

} // CHelper::Node

#endif //CHELPER_NODETARGETSELECTORARGUMENT_H
