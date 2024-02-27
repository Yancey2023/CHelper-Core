//
// Created by Yancey on 2024/2/27.
//

#ifndef CHELPER_NODEHASITEMARGUMENT_H
#define CHELPER_NODEHASITEMARGUMENT_H

#include "../NodeBase.h"
#include "../../resources/id/ItemId.h"

namespace CHelper::Node {

    class NodeHasItemArgument : public NodeBase {
    private:
        std::shared_ptr<NodeBase> nodeItem, nodeSlot;

    public:
        NodeHasItemArgument(const std::optional<std::string> &id,
                            const std::optional<std::string> &description,
                            const std::shared_ptr<NodeBase> &nodeItem,
                            const std::shared_ptr<NodeBase> &nodeSlot);

        ASTNode getASTNode(TokenReader &tokenReader) const override;
    };

} // CHelper::Node

#endif //CHELPER_NODEHASITEMARGUMENT_H
