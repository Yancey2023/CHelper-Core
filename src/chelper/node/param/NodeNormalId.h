//
// Created by Yancey on 2023/11/11.
//

#ifndef CHELPER_NODENORMALID_H
#define CHELPER_NODENORMALID_H

#include "pch.h"
#include "../NodeBase.h"
#include "../../resources/id/NormalId.h"
#include "../../resources/CPack.h"

namespace CHelper::Node {

    class NodeNormalId : public NodeBase {
    public:
        const std::optional<std::string> key;
        const std::shared_ptr<std::vector<std::shared_ptr<NormalId>>> contents;
        const bool allowsMissingID;
        const bool ignoreError;

        ASTNode (*getNormalIdASTNode)(const NodeBase *tokenReader, TokenReader &reader);

        NodeNormalId(const std::optional<std::string> &id,
                     const std::optional<std::string> &description,
                     const std::optional<std::string> &key,
                     bool ignoreError,
                     const std::shared_ptr<std::vector<std::shared_ptr<NormalId>>> &contents,
                     bool allowsMissingID = false,
                     ASTNode(*getNormalIdASTNode)(const NodeBase *node, TokenReader &tokenReader) =
                     [](const NodeBase *node, TokenReader &tokenReader) -> ASTNode {
                         return tokenReader.readStringASTNode(node);
                     });

        NodeNormalId(const nlohmann::json &j,
                     const CPack &cpack);

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

    };

}

#endif //CHELPER_NODENORMALID_H
