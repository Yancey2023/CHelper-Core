//
// Created by Yancey on 2023/11/12.
//

#ifndef CHELPER_NODENAMESPACEID_H
#define CHELPER_NODENAMESPACEID_H


#include "../NodeBase.h"
#include "../../resources/id/NamespaceId.h"
#include "../../resources/CPack.h"
#include <vector>

namespace CHelper::Node {

    class NodeNamespaceId : public NodeBase {
    public:
        const std::optional<std::string> key;
        const std::shared_ptr<std::vector<std::shared_ptr<NamespaceId>>> contents;

        NodeNamespaceId(const std::optional<std::string> &id,
                        const std::optional<std::string> &description,
                        const std::optional<std::string> &key,
                        const std::shared_ptr<std::vector<std::shared_ptr<NamespaceId>>> &contents);

        NodeNamespaceId(const nlohmann::json &j,
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
// CHelper::Node

#endif //CHELPER_NODENAMESPACEID_H
