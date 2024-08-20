//
// Created by Yancey on 2023/11/11.
//

#pragma once

#ifndef CHELPER_NODECOMMAND_H
#define CHELPER_NODECOMMAND_H

#include "../NodeBase.h"
#include "../util/NodeOr.h"
#include "NodePerCommand.h"

namespace CHelper::Node {

    class NodeCommand : public NodeBase {
    private:
        std::vector<std::unique_ptr<Node::NodePerCommand>> *commands = nullptr;

    public:
        NodeCommand(const std::optional<std::u16string> &id,
                    const std::optional<std::u16string> &description,
                    std::vector<std::unique_ptr<Node::NodePerCommand>> *commands);

        NodeCommand() = default;

        void init(const CPack &cpack) override;

        [[nodiscard]] NodeType *getNodeType() const override;

        ASTNode getASTNode(TokenReader &tokenReader, const CPack *cpack) const override;

        std::optional<std::u16string> collectDescription(const ASTNode *node, size_t index) const override;

        bool collectSuggestions(const ASTNode *astNode,
                                size_t index,
                                std::vector<Suggestions> &suggestions) const override;

        void collectStructure(const ASTNode *astNode,
                              StructureBuilder &structure,
                              bool isMustHave) const override;

        bool collectColor(const ASTNode *astNode,
                          ColoredString &coloredString,
                          const Theme &theme) const override;

        CODEC_UNIQUE_PTR_H(NodeCommand)
    };

}// namespace CHelper::Node

#endif//CHELPER_NODECOMMAND_H
