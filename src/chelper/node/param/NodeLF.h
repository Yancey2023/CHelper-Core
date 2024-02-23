//
// Created by Yancey on 2023/11/13.
//

#ifndef CHELPER_NODELF_H
#define CHELPER_NODELF_H

#include "../NodeBase.h"

namespace CHelper::Node {

    class NodeLF : public NodeBase {
    public:
        NODE_TYPE_INSTANCE_H(NodeLF);

        ASTNode getASTNode(TokenReader &tokenReader, const CPack &cpack) const override;

        void collectStructure(const ASTNode *astNode,
                              StructureBuilder &structure,
                              bool isMustHave) const override;

    };

} // CHelper::Node

#endif //CHELPER_NODELF_H
