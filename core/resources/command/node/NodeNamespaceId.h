//
// Created by Yancey666 on 2023/11/12.
//

#ifndef CHELPER_NODENAMESPACEID_H
#define CHELPER_NODENAMESPACEID_H


#include "NodeBase.h"
#include "../../id/NamespaceId.h"
#include "../../CPack.h"
#include <vector>

namespace CHelper::Node {

    class NodeNamespaceId : public NodeBase{
    public:
        const std::optional<std::string> key;
        const std::vector<CHelper::NamespaceId> *contents;

        NodeNamespaceId(const std::optional<std::string> &id,
                     const std::optional<std::string> &description,
                     const std::optional<std::string> &key,
                     const std::vector<NamespaceId> *contents);

        explicit NodeNamespaceId(const nlohmann::json &j,
                                 const CPack &cpack);

        void toJson(nlohmann::json &j) const override;
    };

}
// CHelper::Node

CREATE_ADL_SERIALIZER(CHelper::Node::NodeNamespaceId);

#endif //CHELPER_NODENAMESPACEID_H
