//
// Created by Yancey on 2023/11/11.
//

#pragma once

#ifndef CHELPER_NODENORMALID_H
#define CHELPER_NODENORMALID_H

#include "../../resources/CPack.h"
#include "../../resources/id/NormalId.h"
#include "../NodeBase.h"
#include "pch.h"

namespace CHelper::Node {

    class NodeNormalId : public NodeBase {
    public:
        std::optional<std::string> key;
        std::optional<std::shared_ptr<std::vector<std::shared_ptr<NormalId>>>> contents;
        bool allowMissingID = false;
        std::optional<bool> ignoreError;
        std::function<ASTNode(const NodeBase *node, TokenReader &tokenReader)> getNormalIdASTNode;

    private:
        std::shared_ptr<std::vector<std::shared_ptr<NormalId>>> customContents;

    public:
        NodeNormalId() = default;

        NodeNormalId(
                const std::optional<std::string> &id,
                const std::optional<std::string> &description,
                const std::string &key,
                bool ignoreError,
                bool allowMissingID = false,
                const std::function<ASTNode(const NodeBase *node, TokenReader &tokenReader)> &getNormalIdASTNode =
                        [](const NodeBase *node, TokenReader &tokenReader) -> ASTNode {
                    return tokenReader.readStringOrNumberASTNode(node);
                });

        NodeNormalId(
                const std::optional<std::string> &id,
                const std::optional<std::string> &description,
                bool ignoreError,
                const std::shared_ptr<std::vector<std::shared_ptr<NormalId>>> &contents,
                bool allowMissingID = false,
                const std::function<ASTNode(const NodeBase *node, TokenReader &tokenReader)> &getNormalIdASTNode =
                        [](const NodeBase *node, TokenReader &tokenReader) -> ASTNode {
                    return tokenReader.readStringOrNumberASTNode(node);
                });

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

    CODEC_NODE_H(NodeNormalId)

}// namespace CHelper::Node

#endif//CHELPER_NODENORMALID_H
