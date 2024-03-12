//
// Created by Yancey on 2024/2/27.
//

#ifndef CHELPER_NODEHASPERMISSIONARGUMENT_H
#define CHELPER_NODEHASPERMISSIONARGUMENT_H

#include "../NodeBase.h"

namespace CHelper::Node {

    class NodeHasPermissionArgument : public NodeBase {
    public:
        NodeHasPermissionArgument(const std::optional<std::string> &id,
                                  const std::optional<std::string> &description);

        ASTNode getASTNode(TokenReader &tokenReader, const CPack *cpack) const override;
    };

} // CHelper::Node

#endif //CHELPER_NODEHASPERMISSIONARGUMENT_H
