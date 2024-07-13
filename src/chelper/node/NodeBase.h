//
// Created by Yancey on 2023/11/10.
//

#pragma once

#ifndef CHELPER_NODEBASE_H
#define CHELPER_NODEBASE_H

#include "../lexer/TokenReader.h"
#include "../parser/ASTNode.h"
#include "NodeType.h"
#include "pch.h"

namespace CHelper {

    class CPack;

    namespace Node {

        class NodeType;

        class NodeBase {
        public:
            std::optional<std::string> id;
            std::optional<std::string> brief;
            std::optional<std::string> description;
            //true-一定要在空格后 false-不一定在空格后
            std::optional<bool> isMustAfterWhiteSpace;
            //存储下一个节点，需要调用构造函数之后再进行添加
            std::vector<NodeBase *> nextNodes;

            NodeBase() = default;

            NodeBase(const std::optional<std::string> &id,
                     const std::optional<std::string> &description,
                     bool isMustAfterWhiteSpace);

            virtual ~NodeBase() = default;

            virtual void init(const CPack &cpack);

            [[nodiscard]] HEDLEY_RETURNS_NON_NULL virtual NodeType *getNodeType() const = 0;

            [[nodiscard]] HEDLEY_NON_NULL(3) virtual ASTNode
                    getASTNode(TokenReader &tokenReader, const CPack *cpack) const = 0;

            [[nodiscard]] HEDLEY_NON_NULL(3) ASTNode
                    getASTNodeWithNextNode(TokenReader &tokenReader, const CPack *cpack) const;

            [[nodiscard]] HEDLEY_NON_NULL(3) ASTNode
                    getASTNodeWithNextNode(TokenReader &tokenReader, const CPack *cpack, bool isRequireWhitespace) const;

        protected:
            HEDLEY_NON_NULL(3, 4)
            ASTNode
            getByChildNode(TokenReader &tokenReader,
                           const CPack *cpack,
                           const NodeBase *childNode,
                           const std::string &astNodeId = std::string()) const;

            //node不一定需要的时侯使用
            HEDLEY_NON_NULL(3)
            ASTNode
            getOptionalASTNode(TokenReader &tokenReader,
                               const CPack *cpack,
                               bool isIgnoreChildNodesError,
                               const std::vector<const NodeBase *> &childNodes,
                               const std::string &astNodeId = std::string()) const;

        public:
            [[nodiscard]] bool isAfterWhitespace() const;

            HEDLEY_NON_NULL(2)
            virtual std::optional<std::string> collectDescription(const ASTNode *node, size_t index) const;

            HEDLEY_NON_NULL(2)
            virtual bool collectIdError(const ASTNode *astNode,
                                        std::vector<std::shared_ptr<ErrorReason>> &idErrorReasons) const;

            HEDLEY_NON_NULL(2)
            virtual bool collectSuggestions(const ASTNode *astNode,
                                            size_t index,
                                            std::vector<Suggestions> &suggestions) const;

            virtual void collectStructure(const ASTNode *astNode,
                                          StructureBuilder &structure,
                                          bool isMustHave) const;

            void collectStructureWithNextNodes(StructureBuilder &structure,
                                               bool isMustHave) const;
        };

        CODEC_NODE_H(NodeBase)

        CODEC_UNIQUE_PTR_H(NodeBase)

    }// namespace Node

}// namespace CHelper

#endif//CHELPER_NODEBASE_H
