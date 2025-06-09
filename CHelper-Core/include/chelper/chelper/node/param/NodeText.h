//
// Created by Yancey on 2023/12/2.
//

#pragma once

#ifndef CHELPER_NODETEXT_H
#define CHELPER_NODETEXT_H

#include "../../resources/id/NamespaceId.h"
#include "../NodeBase.h"

namespace CHelper::Node {

    class NodeText : public NodeSerializable {
    public:
        std::optional<std::vector<TokenType::TokenType>> tokenTypes;
        std::shared_ptr<NormalId> data;

    private:
        std::function<ASTNode(const NodeBase *node, TokenReader &tokenReader)> getTextASTNode;

    public:
        NodeText() = default;

        NodeText(
                const std::optional<std::string> &id,
                const std::optional<std::u16string> &description,
                const std::shared_ptr<NormalId> &data,
                const std::function<ASTNode(const NodeBase *node, TokenReader &tokenReader)> &getTextASTNode =
                        [](const NodeBase *node, TokenReader &tokenReader) -> ASTNode {
                    return tokenReader.readStringASTNode(node);
                });

        void init(const CPack &cpack) override;

        [[nodiscard]] NodeTypeId::NodeTypeId getNodeType() const override;

        ASTNode getASTNode(TokenReader &tokenReader, const CPack *cpack = nullptr) const override;

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

CODEC_NODE(CHelper::Node::NodeText, tokenTypes, data)

#endif//CHELPER_NODETEXT_H
