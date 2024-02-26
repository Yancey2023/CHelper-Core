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
        std::shared_ptr<NodeBase> nodeItem, nodeFamily, nodeGameMode, nodeItemLocation;

    public:
        NodeTargetSelectorArgument(const std::optional<std::string> &id,
                                   const std::optional<std::string> &description,
                                   const std::shared_ptr<NodeBase> &nodeItem,
                                   const std::shared_ptr<NodeBase> &nodeFamily,
                                   const std::shared_ptr<NodeBase> &nodeGameMode,
                                   const std::shared_ptr<NodeBase> &nodeItemLocation);

        ASTNode getASTNode(TokenReader &tokenReader) const override;
    };

} // CHelper::Node

#endif //CHELPER_NODETARGETSELECTORARGUMENT_H
