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
        std::shared_ptr<std::vector<std::shared_ptr<NamespaceId>>> contents;
        bool ignoreError;

        NodeNamespaceId(const std::optional<std::string> &id,
                        const std::optional<std::string> &description,
                        const std::optional<std::string> &key,
                        bool ignoreError,
                        const std::shared_ptr<std::vector<std::shared_ptr<NamespaceId>>> &contents);

        NodeNamespaceId(const nlohmann::json &j,
                        const CPack &cpack);

        NodeNamespaceId(BinaryReader &binaryReader,
                        [[maybe_unused]] const CPack &cpack);

        [[nodiscard]] NodeType *getNodeType() const override;

        void toJson(nlohmann::json &j) const override;

        ASTNode getASTNode(TokenReader &tokenReader, const CPack *cpack) const override;

        bool collectIdError(const ASTNode *astNode,
                            std::vector<std::shared_ptr<ErrorReason>> &idErrorReasons) const override;

        bool collectSuggestions(const ASTNode *astNode,
                                size_t index,
                                std::vector<Suggestions> &suggestions) const override;

        void collectStructure(const ASTNode *astNode,
                              StructureBuilder &structure,
                              bool isMustHave) const override;
        void writeBinToFile(BinaryWriter &binaryWriter) const override;
    };

}// namespace CHelper::Node
// CHelper::Node

#endif//CHELPER_NODENAMESPACEID_H
