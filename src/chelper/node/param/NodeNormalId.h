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

        ASTNode (*getNormalIdASTNode)(const NodeBase *tokenReader, TokenReader &reader);

        NodeNormalId(const std::optional<std::string> &id,
                     const std::optional<std::string> &description,
                     const std::optional<std::string> &key,
                     const std::shared_ptr<std::vector<std::shared_ptr<NormalId>>> &contents,
                     ASTNode(*getNormalIdASTNode)(const NodeBase *node, TokenReader &tokenReader) =
                     [](const NodeBase *node, TokenReader &tokenReader) -> ASTNode {
                         return tokenReader.getStringASTNode(node);
                     });

        NodeNormalId(const nlohmann::json &j,
                     const CPack &cpack);

        [[nodiscard]] NodeType getNodeType() const override;

        void toJson(nlohmann::json &j) const override;

        ASTNode getASTNode(TokenReader &tokenReader) const override;

        bool collectIdError(const ASTNode *astNode,
                            std::vector<std::shared_ptr<ErrorReason>> &idErrorReasons) const override;

        bool collectSuggestions(const ASTNode *astNode,
                                size_t index,
                                std::vector<Suggestion> &suggestions) const override;

        void collectStructure(const ASTNode *astNode,
                              StructureBuilder &structure,
                              bool isMustHave) const override;

    };

}

#endif //CHELPER_NODENORMALID_H
