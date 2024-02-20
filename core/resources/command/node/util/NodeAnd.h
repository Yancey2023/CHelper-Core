//
// Created by Yancey666 on 2023/12/22.
//

#ifndef CHELPER_NODEAND_H
#define CHELPER_NODEAND_H

#include "../NodeBase.h"

namespace CHelper::Node {

    class NodeAnd : public NodeBase {
    public:
        const std::vector<std::shared_ptr<NodeBase>> childNodes;

        NodeAnd(const std::optional<std::string> &id,
                const std::optional<std::string> &description,
                const std::vector<std::shared_ptr<NodeBase>> &childNodes);

        NodeAnd(const nlohmann::json &j,
                const CPack &cpack);

        NODE_TYPE_H

        ASTNode getASTNode(TokenReader &tokenReader, const CPack &cpack) const override;

    };

} // CHelper::Node

#endif //CHELPER_NODEAND_H
