//
// Created by Yancey on 2023/11/11.
//

#pragma once

#ifndef CHELPER_NODECOMMANDNAME_H
#define CHELPER_NODECOMMANDNAME_H

#include "../NodeBase.h"
#include "../NodeType.h"
#include "NodePerCommand.h"

namespace CHelper::Node {

    class NodeCommandName : public NodeBase {
    private:
        std::vector<std::unique_ptr<Node::NodePerCommand>> *commands = nullptr;

    public:
        void init(const CPack &cpack) override;

        [[nodiscard]] NodeType *getNodeType() const override;

        ASTNode getASTNode(TokenReader &tokenReader, const CPack *cpack) const override;

        bool collectIdError(const ASTNode *astNode,
                            std::vector<std::shared_ptr<ErrorReason>> &idErrorReasons) const override;

        bool collectSuggestions(const ASTNode *astNode,
                                size_t index,
                                std::vector<Suggestions> &suggestions) const override;

        void collectStructure(const ASTNode *astNode,
                              StructureBuilder &structure,
                              bool isMustHave) const override;

        bool collectColor(const ASTNode *astNode,
                          ColoredString &coloredString,
                          const Theme &theme) const override;
    };

    CODEC_UNIQUE_PTR_H(NodeCommandName)

}// namespace CHelper::Node

#endif//CHELPER_NODECOMMANDNAME_H
