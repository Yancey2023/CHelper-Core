//
// Created by Yancey on 2023/11/13.
//

#pragma once

#ifndef CHELPER_NODELF_H
#define CHELPER_NODELF_H

#include "../NodeBase.h"

namespace CHelper::Node {

    class NodeLF : public NodeBase {
    public:
        NodeLF(const std::optional<std::string> &id,
               const std::optional<std::string> &description);

        [[nodiscard]] NodeType *getNodeType() const override;

        static NodeLF *getInstance();

        ASTNode getASTNode(TokenReader &tokenReader, const CPack *cpack) const override;

        void collectStructure(const ASTNode *astNode,
                              StructureBuilder &structure,
                              bool isMustHave) const override;
    };

}// namespace CHelper::Node

#endif//CHELPER_NODELF_H
