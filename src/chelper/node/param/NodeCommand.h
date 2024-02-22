//
// Created by Yancey666 on 2023/11/11.
//

#ifndef CHELPER_NODECOMMAND_H
#define CHELPER_NODECOMMAND_H

#include "../NodeBase.h"
#include "../util/NodeOr.h"

namespace CHelper::Node {

    class NodeCommand : public NodeBase {

    public:
        NodeCommand(const std::optional<std::string> &id,
                    const std::optional<std::string> &description,
                    const std::vector<std::shared_ptr<Node::NodeBase>> &nodeCommand);

        NodeCommand(const nlohmann::json &j,
                    const CPack &cpack);

        NODE_TYPE_H;

        ASTNode getASTNode(TokenReader &tokenReader, const CPack &cpack) const override;

        std::optional<std::string> getDescription(const ASTNode *node, size_t index) const override;

    };

} // CHelper::Node

CREATE_ADL_SERIALIZER(CHelper::Node::NodeCommand);

#endif //CHELPER_NODECOMMAND_H