//
// Created by Yancey on 2023/12/2.
//

#pragma once

#ifndef CHELPER_NODETEXT_H
#define CHELPER_NODETEXT_H

#include "../NodeBase.h"
#include "../../resources/id/NamespaceId.h"

namespace CHelper::Node {

    class NodeText : public NodeBase {
        const std::shared_ptr<NormalId> data;
        const std::function<ASTNode(const NodeBase *node, TokenReader &tokenReader)> getTextASTNode;

    public:
        NodeText(const std::optional<std::string> &id,
                 const std::optional<std::string> &description,
                 const std::shared_ptr<NormalId> &data,
                 const std::function<ASTNode(const NodeBase *node, TokenReader &tokenReader)>& getTextASTNode =
                 [](const NodeBase *node, TokenReader &tokenReader) -> ASTNode {
                     return tokenReader.readStringASTNode(node);
                 });

        NodeText(const nlohmann::json &j,
                 [[maybe_unused]] const CPack &cpack);

        [[nodiscard]] NodeType *getNodeType() const override;

        void toJson(nlohmann::json &j) const override;

        ASTNode getASTNode(TokenReader &tokenReader, const CPack *cpack) const override;

        bool collectSuggestions(const ASTNode *astNode,
                                size_t index,
                                std::vector<Suggestions> &suggestions) const override;

        void collectStructure(const ASTNode *astNode,
                              StructureBuilder &structure,
                              bool isMustHave) const override;

    };
} // CHelper::Node

#endif //CHELPER_NODETEXT_H
