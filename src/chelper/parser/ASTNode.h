//
// Created by Yancey on 2023/12/15.
//

#pragma once

#ifndef CHELPER_ASTNODE_H
#define CHELPER_ASTNODE_H

#include "../lexer/Token.h"
#include "../util/VectorView.h"
#include "ErrorReason.h"
#include "StructureBuilder.h"
#include "Suggestion.h"
#include "Suggestions.h"
#include "pch.h"

namespace CHelper {

    class CPack;

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
    }

    class ASTNode {
    public:
        ASTNodeMode::ASTNodeMode mode;
        //子节点为AND类型和OR类型特有
        std::vector<ASTNode> childNodes;
        VectorView<Token> tokens;
        //一个Node可能会生成多个ASTNode，这些ASTNode使用id进行区分
        std::string id;
        const Node::NodeBase *node;
        //不要直接用这个，这里不包括ID错误，只有结构错误，应该用getErrorReason()
        std::vector<std::shared_ptr<ErrorReason>> errorReasons;
        //哪个节点最好，OR类型特有，获取颜色和生成命令格式文本的时候使用
        size_t whichBest;
        //是否需要加后面加空格
        bool canAddWhitespace;

        ASTNode(ASTNodeMode::ASTNodeMode mode,
                const Node::NodeBase *node,
                std::vector<ASTNode> &&childNodes,
                const VectorView<Token> &tokens,
                const std::vector<std::shared_ptr<ErrorReason>> &errorReasons,
                std::string id,
                bool canAddWhitespace,
                size_t whichBest = -1);

    public:
        [[nodiscard]] nlohmann::json toJson() const;

        [[nodiscard]] nlohmann::json toBestJson() const;

        static ASTNode simpleNode(const Node::NodeBase *node,
                                  const VectorView<Token> &tokens,
                                  const std::shared_ptr<ErrorReason> &errorReason = nullptr,
                                  const std::string &id = std::string(),
                                  bool canAddWhitespace = true);

        static ASTNode andNode(const Node::NodeBase *node,
                               std::vector<ASTNode> &&childNodes,
                               const VectorView<Token> &tokens,
                               const std::shared_ptr<ErrorReason> &errorReason = nullptr,
                               const std::string &id = std::string(),
                               bool canAddWhitespace = true);

        static ASTNode orNode(const Node::NodeBase *node,
                              std::vector<ASTNode> &&childNodes,
                              const VectorView<Token> *tokens,
                              const char *errorReason = nullptr,
                              const std::string &id = std::string(),
                              bool canAddWhitespace = true);

        static ASTNode orNode(const Node::NodeBase *node,
                              std::vector<ASTNode> &&childNodes,
                              const VectorView<Token> &tokens,
                              const char *errorReason = nullptr,
                              const std::string &id = std::string(),
                              bool canAddWhitespace = true);

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

        [[nodiscard]] std::optional<std::string> collectDescription(size_t index) const;

        void collectIdErrors(std::vector<std::shared_ptr<ErrorReason>> &idErrorReasons) const;

        void collectSuggestions(size_t index, std::vector<Suggestions> &suggestions) const;

        void collectStructure(StructureBuilder &structureBuilder, bool isMustHave) const;

        [[nodiscard]] std::string getDescription(size_t index) const;

        [[nodiscard]] std::vector<std::shared_ptr<ErrorReason>> getIdErrors() const;

        [[nodiscard]] std::vector<std::shared_ptr<ErrorReason>> getErrorReasons() const;

        [[nodiscard]] std::vector<Suggestion> getSuggestions(size_t index) const;

        [[nodiscard]] std::string getStructure() const;

        [[nodiscard]] std::string getColors() const;
    };

}// namespace CHelper


#endif//CHELPER_ASTNODE_H
