//
// Created by Yancey666 on 2023/12/22.
//

#ifndef CHELPER_NODEENTRY_H
#define CHELPER_NODEENTRY_H


#include "../NodeBase.h"

namespace CHelper::Node {

    class NodeEntry : public NodeBase {
    public:
        std::shared_ptr<NodeBase> nodeKey;
        std::shared_ptr<NodeBase> nodeSeparator;
        std::shared_ptr<NodeBase> nodeValue;

        NodeEntry(const std::optional<std::string> &id,
                  const std::optional<std::string> &description,
                  std::shared_ptr<NodeBase> &nodeKey,
                  std::shared_ptr<NodeBase> &nodeSeparator,
                  std::shared_ptr<NodeBase> &nodeValue);

        NodeEntry(const nlohmann::json &j,
                  const CPack &cpack);

        NODE_TYPE_H;

        ASTNode getASTNode(TokenReader &tokenReader, const CPack &cpack) const override;

    };

} // CHelper::Node


#endif //CHELPER_NODEENTRY_H
