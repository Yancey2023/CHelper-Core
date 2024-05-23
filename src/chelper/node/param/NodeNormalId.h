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
        std::shared_ptr<std::vector<std::shared_ptr<NormalId>>> contents;
        bool allowsMissingID;
        bool ignoreError;
        std::function<ASTNode(const NodeBase *node, TokenReader &tokenReader)> getNormalIdASTNode;

        NodeNormalId(const std::optional<std::string> &id, const std::optional<std::string> &description, const std::optional<std::string> &key, bool ignoreError, const std::shared_ptr<std::vector<std::shared_ptr<NormalId>>> &contents, bool allowsMissingID = false, const std::function<ASTNode(const NodeBase *node, TokenReader &tokenReader)> &getNormalIdASTNode = [](const NodeBase *node, TokenReader &tokenReader) -> ASTNode { return tokenReader.readStringASTNode(node); });

        NodeNormalId(const nlohmann::json &j,
                     const CPack &cpack);

        NodeNormalId(BinaryReader &binaryReader,
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

#endif//CHELPER_NODENORMALID_H
