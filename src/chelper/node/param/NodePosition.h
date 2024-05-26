//
// Created by Yancey on 2023/12/2.
//

#pragma once

#ifndef CHELPER_NODEPOSITION_H
#define CHELPER_NODEPOSITION_H

#include "../NodeBase.h"

namespace CHelper::Node {

    class NodePosition : public NodeBase {
    public:
        NodePosition(const std::optional<std::string> &id,
                     const std::optional<std::string> &description);

        NodePosition() = default;

        [[nodiscard]] NodeType *getNodeType() const override;

        ASTNode getASTNode(TokenReader &tokenReader, const CPack *cpack) const override;

        bool collectIdError(const ASTNode *astNode,
                            std::vector<std::shared_ptr<ErrorReason>> &idErrorReasons) const override;

        bool collectSuggestions(const ASTNode *astNode, size_t index, std::vector<Suggestions> &suggestions) const override;

        void collectStructure(const ASTNode *astNode,
                              StructureBuilder &structure,
                              bool isMustHave) const override;
    };

    CODEC_UNIQUE_PTR_H(NodePosition)

}// namespace CHelper::Node

#endif//CHELPER_NODEPOSITION_H
