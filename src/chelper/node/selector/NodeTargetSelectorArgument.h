//
// Created by Yancey on 2024/2/20.
//

#ifndef CHELPER_NODETARGETSELECTORARGUMENT_H
#define CHELPER_NODETARGETSELECTORARGUMENT_H

#include "../NodeBase.h"

namespace CHelper::Node {

    class NodeTargetSelectorArgument : public NodeBase {
    public:
        NodeTargetSelectorArgument(const std::optional<std::string> &id,
                                   const std::optional<std::string> &description);

        ASTNode getASTNode(TokenReader &tokenReader, const CPack &cpack) const override;
    };

} // CHelper::Node

#endif //CHELPER_NODETARGETSELECTORARGUMENT_H
