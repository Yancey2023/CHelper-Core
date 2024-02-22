//
// Created by Yancey666 on 2023/11/12.
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
        const std::vector<std::shared_ptr<CHelper::NamespaceId>> *contents;

        NodeNamespaceId(const std::optional<std::string> &id,
                        const std::optional<std::string> &description,
                        const std::optional<std::string> &key,
                        const std::vector<std::shared_ptr<NamespaceId>> *contents);

        NodeNamespaceId(const nlohmann::json &j,
                        const CPack &cpack);

        ~NodeNamespaceId() override;

        NODE_TYPE_H;

        void toJson(nlohmann::json &j) const override;

        ASTNode getASTNode(TokenReader &tokenReader, const CPack &cpack) const override;

        bool collectIdError(const ASTNode *astNode,
                            std::vector<std::shared_ptr<ErrorReason>> &idErrorReasons) const override;
    };

}
// CHelper::Node

CREATE_ADL_SERIALIZER(CHelper::Node::NodeNamespaceId);

#endif //CHELPER_NODENAMESPACEID_H
