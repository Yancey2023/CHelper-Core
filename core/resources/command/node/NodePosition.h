//
// Created by Yancey666 on 2023/12/2.
//

#ifndef CHELPER_NODEPOSITION_H
#define CHELPER_NODEPOSITION_H

#include "NodeBase.h"

namespace CHelper::Node {

    class NodePosition : public NodeBase {
    public:
        NodePosition(const std::optional<std::string> &id,
                     const std::optional<std::string> &description);

        NodePosition(const nlohmann::json &j,
                     const CPack &cpack);

        NODE_TYPE_H;

        ASTNode getASTNode(TokenReader &tokenReader, const CPack &cpack) const override;
    };

} // CHelper::Node

#endif //CHELPER_NODEPOSITION_H
