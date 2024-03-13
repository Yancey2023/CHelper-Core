//
// Created by Yancey on 2023/12/10.
//

#ifndef CHELPER_NODESTRING_H
#define CHELPER_NODESTRING_H

#include "pch.h"
#include "../NodeBase.h"

namespace CHelper::Node {

    class NodeString : public NodeBase {
    public:
        const bool allowMissingString;
        const bool canContainSpace;
        const bool ignoreLater;

        NodeString(const std::optional<std::string> &id,
                   const std::optional<std::string> &description,
                   bool allowMissingString,
                   bool canContainSpace,
                   bool ignoreLater);

        NodeString(const nlohmann::json &j,
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

#endif //CHELPER_NODESTRING_H
