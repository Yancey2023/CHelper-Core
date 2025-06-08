//
// Created by Yancey on 2023/12/20.
//

#pragma once

#ifndef CHELPER_NODERELATIVEFLOAT_H
#define CHELPER_NODERELATIVEFLOAT_H

#include "../NodeBase.h"

namespace CHelper::Node {

    namespace NodeRelativeFloatType {
        enum NodeRelativeFloatType : uint8_t {
            ABSOLUTE_COORDINATE,
            RELATIVE_WORLD_COORDINATE,
            LOCAL_COORDINATE,
        };
    }

    class NodeRelativeFloat : public NodeBase {
    public:
        bool canUseCaretNotation = true;

        NodeRelativeFloat() = default;

        NodeRelativeFloat(const std::optional<std::u16string> &id,
                          const std::optional<std::u16string> &description,
                          bool canUseCaretNotation);

        [[nodiscard]] NodeTypeId::NodeTypeId getNodeType() const override;

        ASTNode getASTNode(TokenReader &tokenReader, const CPack *cpack) const override;

        bool collectIdError(const ASTNode *astNode,
                            std::vector<std::shared_ptr<ErrorReason>> &idErrorReasons) const override;

        static std::pair<NodeRelativeFloatType::NodeRelativeFloatType, ASTNode>
        getASTNode(const NodeBase *node,
                   const CPack *cpack,
                   TokenReader &tokenReader);

        bool collectSuggestions(const ASTNode *astNode, size_t index, std::vector<Suggestions> &suggestions) const override;

        static bool collectSuggestions(size_t index, std::vector<Suggestions> &suggestions, bool canUseCaretNotation);

        void collectStructure(const ASTNode *astNode,
                              StructureBuilder &structure,
                              bool isMustHave) const override;

        bool collectSyntax(const ASTNode *astNode,
                           SyntaxResult &syntaxResult) const override;
    };

}// namespace CHelper::Node

CODEC_NODE(CHelper::Node::NodeRelativeFloat, canUseCaretNotation)

#endif//CHELPER_NODERELATIVEFLOAT_H
