//
// Created by Yancey on 2023/11/11.
//

#pragma once

#ifndef CHELPER_NODEBOOLEAN_H
#define CHELPER_NODEBOOLEAN_H

#include "../NodeBase.h"

namespace CHelper::Node {

    template<bool isJson>
    class NodeTemplateBoolean : public NodeBase {
    public:
        std::optional<std::u16string> descriptionTrue, descriptionFalse;

        NodeTemplateBoolean() = default;

        [[nodiscard]] NodeTypeId::NodeTypeId getNodeType() const override {
            if constexpr (!isJson) {
                return NodeTypeId::BOOLEAN;
            } else {
                return NodeTypeId::JSON_BOOLEAN;
            }
        }

        ASTNode getASTNode(TokenReader &tokenReader, const CPack *cpack) const override {
            ASTNode astNode = tokenReader.readStringASTNode(this);
            std::u16string_view str = astNode.tokens.toString();
            if (HEDLEY_LIKELY(str == u"true" || str == u"false")) {
                return astNode;
            }
            TokensView tokens = astNode.tokens;
            return ASTNode::andNode(this, {std::move(astNode)}, tokens,
                                    ErrorReason::contentError(tokens, fmt::format(u"内容不匹配，应该为布尔值，但当前内容为{}", str)));
        }

        bool collectSuggestions(const ASTNode *astNode,
                                size_t index,
                                std::vector<Suggestions> &suggestions) const override {
            KMPMatcher kmpMatcher(astNode->tokens.toString().substr(0, index - astNode->tokens.getStartIndex()));
            Suggestions suggestions1(SuggestionsType::LITERAL);
            if (HEDLEY_UNLIKELY(kmpMatcher.match(u"true") != std::u16string::npos)) {
                suggestions1.suggestions.emplace_back(
                        astNode->tokens, true,
                        NormalId::make(u"true", descriptionTrue));
            }
            if (HEDLEY_UNLIKELY(kmpMatcher.match(u"false") != std::u16string::npos)) {
                suggestions1.suggestions.emplace_back(
                        astNode->tokens, true,
                        NormalId::make(u"false", descriptionFalse));
            }
            suggestions1.markFiltered();
            suggestions.push_back(std::move(suggestions1));
            return true;
        }

        void collectStructure(const ASTNode *astNode,
                              StructureBuilder &structure,
                              bool isMustHave) const override {
            structure.append(isMustHave, description.value_or(u"布尔值"));
        }

        bool collectSyntax(const ASTNode *astNode,
                           SyntaxResult &syntaxResult) const override {
            syntaxResult.update(astNode->tokens, SyntaxTokenType::BOOLEAN);
            return true;
        }

        static std::unique_ptr<NodeTemplateBoolean> make(const std::optional<std::u16string> &id,
                                                         const std::optional<std::u16string> &description,
                                                         const std::optional<std::u16string> &descriptionTrue,
                                                         const std::optional<std::u16string> &descriptionFalse) {
            auto node = std::make_unique<NodeTemplateBoolean>();
            node->id = id;
            node->description = description;
            node->isMustAfterWhiteSpace = false;
            node->descriptionTrue = descriptionTrue;
            node->descriptionFalse = descriptionFalse;
            return node;
        }
    };

    using NodeBoolean = NodeTemplateBoolean<false>;
    using NodeJsonBoolean = NodeTemplateBoolean<true>;

}// namespace CHelper::Node

CODEC_NODE(CHelper::Node::NodeBoolean, descriptionTrue, descriptionFalse)
CODEC_NODE(CHelper::Node::NodeJsonBoolean, descriptionTrue, descriptionFalse)

#endif//CHELPER_NODEBOOLEAN_H
