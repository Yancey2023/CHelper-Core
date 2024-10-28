//
// Created by Yancey on 2023/12/15.
//

#pragma once

#ifndef CHELPER_ASTNODE_H
#define CHELPER_ASTNODE_H

#include "../lexer/Token.h"
#include "StructureBuilder.h"
#include "Suggestions.h"
#include "pch.h"
#include <chelper/parser/ColoredString.h>
#include <chelper/parser/ErrorReason.h>
#include <chelper/settings/Settings.h>

namespace CHelper {

    static std::shared_ptr<NormalId> whitespaceId = NormalId::make(u" ", u"空格");

    namespace Node {

        class NodeBase;

    }// namespace Node

    namespace ASTNodeMode {
        enum ASTNodeMode : uint8_t {
            //没有向下的分支
            NONE,
            //有向下的分支，子节点为and关系
            AND,
            //有向下的分支，子节点为or关系
            OR
        };
    }// namespace ASTNodeMode

    namespace ASTNodeId {
        enum ASTNodeId : uint8_t {
            NONE,
            COMPOUND,
            NEXT_NODE,
            NODE_JSON_ALL_LIST,
            NODE_STRING_INNER,
            NODE_BLOCK_BLOCK_ID,
            NODE_BLOCK_BLOCK_STATE,
            NODE_BLOCK_BLOCK_AND_BLOCK_STATE,
            NODE_COMMAND_COMMAND_NAME,
            NODE_COMMAND_COMMAND,
            NODE_POSITION_POSITIONS,
            NODE_POSITION_POSITIONS_WITH_ERROR,
            NODE_TARGET_SELECTOR_PLAYER_NAME,
            NODE_TARGET_SELECTOR_WITH_ARGUMENTS,
            NODE_TARGET_SELECTOR_NO_ARGUMENTS,
            NODE_RELATIVE_FLOAT_NUMBER,
            NODE_RELATIVE_FLOAT_WITH_ERROR,
        };
    }// namespace ASTNodeId

    class ASTNode {
    public:
        ASTNodeMode::ASTNodeMode mode;
        //子节点为AND类型和OR类型特有
        std::vector<ASTNode> childNodes;
        TokensView tokens;
        //一个Node可能会生成多个ASTNode，这些ASTNode使用id进行区分
        ASTNodeId::ASTNodeId id;
        const Node::NodeBase *node;
        //不要直接用这个，这里不包括ID错误，只有结构错误，应该用getErrorReason()
        std::vector<std::shared_ptr<ErrorReason>> errorReasons;
        //哪个节点最好，OR类型特有，获取颜色和生成命令格式文本的时候使用
        size_t whichBest;

        ASTNode(ASTNodeMode::ASTNodeMode mode,
                const Node::NodeBase *node,
                std::vector<ASTNode> &&childNodes,
                TokensView tokens,
                const std::vector<std::shared_ptr<ErrorReason>> &errorReasons,
                ASTNodeId::ASTNodeId id,
                size_t whichBest = -1);


        class CPack;

    public:
#if CHelperTest == true
        [[nodiscard]]json toJson() const;

        [[nodiscard]]json toBestJson() const;
#endif

        static ASTNode simpleNode(const Node::NodeBase *node,
                                  const TokensView &tokens,
                                  const std::shared_ptr<ErrorReason> &errorReason = nullptr,
                                  const ASTNodeId::ASTNodeId &id = ASTNodeId::NONE);

        static ASTNode andNode(const Node::NodeBase *node,
                               std::vector<ASTNode> &&childNodes,
                               const TokensView &tokens,
                               const std::shared_ptr<ErrorReason> &errorReason = nullptr,
                               const ASTNodeId::ASTNodeId &id = ASTNodeId::NONE);

        // TODO 为什么当时我用的是char*，而不是std::shared_ptr<ErrorReason>

        static ASTNode orNode(const Node::NodeBase *node,
                              std::vector<ASTNode> &&childNodes,
                              const TokensView *tokens,
                              const char16_t *errorReason = nullptr,
                              const ASTNodeId::ASTNodeId &id = ASTNodeId::NONE);

        static ASTNode orNode(const Node::NodeBase *node,
                              std::vector<ASTNode> &&childNodes,
                              const TokensView &tokens,
                              const char16_t *errorReason = nullptr,
                              const ASTNodeId::ASTNodeId &id = ASTNodeId::NONE);

        //是否有结构错误（不包括ID错误）
        [[nodiscard]] bool isError() const {
            return !errorReasons.empty();
        }

        [[nodiscard]] bool hasChildNode() const {
            return !childNodes.empty();
        }

        [[nodiscard]] bool isAllWhitespaceError() const;

        [[nodiscard]] const ASTNode &getBestNode() const {
            return childNodes[whichBest];
        }

        [[nodiscard]] std::optional<std::u16string> collectDescription(size_t index) const;

        void collectIdErrors(std::vector<std::shared_ptr<ErrorReason>> &idErrorReasons) const;

        void collectSuggestions(size_t index, std::vector<Suggestions> &suggestions) const;

        void collectStructure(StructureBuilder &structureBuilder, bool isMustHave) const;

        void collectColor(ColoredString &coloredString, const Theme &theme) const;

        [[nodiscard]] std::u16string getDescription(size_t index) const;

        [[nodiscard]] std::vector<std::shared_ptr<ErrorReason>> getIdErrors() const;

        [[nodiscard]] std::vector<std::shared_ptr<ErrorReason>> getErrorReasons() const;

        [[nodiscard]] std::vector<Suggestion> getSuggestions(size_t index) const;

        [[nodiscard]] std::u16string getStructure() const;

        [[nodiscard]] ColoredString getColors(const Theme &theme) const;
    };

}// namespace CHelper

#endif//CHELPER_ASTNODE_H
