//
// Created by Yancey on 2023/12/20.
//

#pragma once

#ifndef CHELPER_NODERELATIVEFLOAT_H
#define CHELPER_NODERELATIVEFLOAT_H

#include "../NodeBase.h"

namespace CHelper::Node {

    class NodeRelativeFloat : public NodeBase {
    public:
        bool canUseCaretNotation = true;

        NodeRelativeFloat() = default;

        NodeRelativeFloat(const std::optional<std::u16string> &id,
                          const std::optional<std::u16string> &description,
                          bool canUseCaretNotation);

        [[nodiscard]] NodeType *getNodeType() const override;

        ASTNode getASTNode(TokenReader &tokenReader, const CPack *cpack) const override;

        bool collectIdError(const ASTNode *astNode,
                            std::vector<std::shared_ptr<ErrorReason>> &idErrorReasons) const override;

        static std::pair<uint8_t, ASTNode> getASTNode(const NodeBase *node,
                                                      const CPack *cpack,
                                                      TokenReader &tokenReader);

        bool collectSuggestions(const ASTNode *astNode, size_t index, std::vector<Suggestions> &suggestions) const override;

        static bool collectSuggestions(size_t index, std::vector<Suggestions> &suggestions, bool canUseCaretNotation);

        void collectStructure(const ASTNode *astNode,
                              StructureBuilder &structure,
                              bool isMustHave) const override;

        bool collectColor(const ASTNode *astNode,
                          ColoredString &coloredString,
                          const Theme &theme) const override;
    };

    CODEC_NODE_H(NodeRelativeFloat)

}// namespace CHelper::Node

#endif//CHELPER_NODERELATIVEFLOAT_H
