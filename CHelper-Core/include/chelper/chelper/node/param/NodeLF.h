//
// Created by Yancey on 2023/11/13.
//

#pragma once

#ifndef CHELPER_NODELF_H
#define CHELPER_NODELF_H

#include "../NodeBase.h"
#include "../util/NodeWrapped.h"

namespace CHelper::Node {

    class NodeLF : public NodeSerializable {
    public:
        NodeLF() = default;

        NodeLF(const std::optional<std::string> &id,
               const std::optional<std::u16string> &description);

        [[nodiscard]] NodeTypeId::NodeTypeId getNodeType() const override;

        static NodeWrapped *getInstance();

        ASTNode getASTNode(TokenReader &tokenReader, const CPack *cpack = nullptr) const override;

        void collectStructure(const ASTNode *astNode,
                              StructureBuilder &structure,
                              bool isMustHave) const override;
    };

}// namespace CHelper::Node

#endif//CHELPER_NODELF_H
