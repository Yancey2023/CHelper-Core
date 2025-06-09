//
// Created by Yancey on 2023/11/11.
//

#pragma once

#ifndef CHELPER_NODECOMMAND_H
#define CHELPER_NODECOMMAND_H

#include "../NodeBase.h"
#include "NodePerCommand.h"

namespace CHelper::Node {

    class NodeCommand : public NodeSerializable {
    private:
        std::vector<std::unique_ptr<NodePerCommand>> *commands = nullptr;

    public:
        NodeCommand(const std::optional<std::string> &id,
                    const std::optional<std::u16string> &description,
                    std::vector<std::unique_ptr<NodePerCommand>> *commands);

        NodeCommand() = default;

        void init(const CPack &cpack) override;

        [[nodiscard]] NodeTypeId::NodeTypeId getNodeType() const override;

        ASTNode getASTNode(TokenReader &tokenReader, const CPack *cpack = nullptr) const override;

        std::optional<std::u16string> collectDescription(const ASTNode *node, size_t index) const override;

        bool collectSuggestions(const ASTNode *astNode,
                                size_t index,
                                std::vector<Suggestions> &suggestions) const override;

        void collectStructure(const ASTNode *astNode,
                              StructureBuilder &structure,
                              bool isMustHave) const override;

        bool collectSyntax(const ASTNode *astNode,
                          SyntaxResult &syntaxResult) const override;
    };

}// namespace CHelper::Node

CODEC_NODE_NONE(CHelper::Node::NodeCommand)

#endif//CHELPER_NODECOMMAND_H
