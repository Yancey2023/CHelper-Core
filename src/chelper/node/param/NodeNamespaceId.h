//
// Created by Yancey on 2023/11/12.
//

#pragma once

#ifndef CHELPER_NODENAMESPACEID_H
#define CHELPER_NODENAMESPACEID_H


#include "../../resources/CPack.h"
#include "../../resources/id/NamespaceId.h"
#include "../NodeBase.h"

namespace CHelper::Node {

    class NodeNamespaceId : public NodeBase {
    public:
        std::optional<std::string> key;
        std::optional<std::shared_ptr<std::vector<std::shared_ptr<NamespaceId>>>> contents;
        std::optional<bool> ignoreError;

    private:
        std::shared_ptr<std::vector<std::shared_ptr<NamespaceId>>> customContents;

    public:
        NodeNamespaceId() = default;

        NodeNamespaceId(const std::optional<std::string> &id,
                        const std::optional<std::string> &description,
                        const std::optional<std::string> &key,
                        bool ignoreError);

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
        void init(const CPack &cpack) override;
    };

    CODEC_NODE_H(NodeNamespaceId)

}// namespace CHelper::Node

#endif//CHELPER_NODENAMESPACEID_H
