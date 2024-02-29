//
// Created by Yancey on 2023/12/2.
//

#ifndef CHELPER_NODETEXT_H
#define CHELPER_NODETEXT_H

#include "../NodeBase.h"
#include "../../resources/id/NamespaceId.h"

namespace CHelper::Node {

    class NodeText : public NodeBase {
        const std::shared_ptr<NormalId> data;

        ASTNode (*getTextASTNode)(const NodeBase *tokenReader, TokenReader &reader);

    public:
        NodeText(const std::optional<std::string> &id,
                 const std::optional<std::string> &description,
                 const std::shared_ptr<NormalId> &data,
                 ASTNode(*getNormalIdASTNode)(const NodeBase *node, TokenReader &tokenReader) =
                 [](const NodeBase *node, TokenReader &tokenReader) -> ASTNode {
                     return tokenReader.readStringASTNode(node);
                 });

        explicit NodeText(const nlohmann::json &j,
                          [[maybe_unused]] const CPack &cpack);

        [[nodiscard]] std::shared_ptr<NodeType> getNodeType() const override;

        void toJson(nlohmann::json &j) const override;

        ASTNode getASTNode(TokenReader &tokenReader, const CPack &cpack) const override;

        bool collectSuggestions(const ASTNode *astNode,
                                size_t index,
                                std::vector<Suggestion> &suggestions) const override;

        void collectStructure(const ASTNode *astNode,
                              StructureBuilder &structure,
                              bool isMustHave) const override;

    };
} // CHelper::Node

#endif //CHELPER_NODETEXT_H
