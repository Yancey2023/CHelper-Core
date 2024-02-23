//
// Created by Yancey on 2023/11/11.
//

#ifndef CHELPER_NODEITEM_H
#define CHELPER_NODEITEM_H

#include "../NodeBase.h"
#include "NodeNamespaceId.h"

namespace CHelper::Node {

    namespace NodeItemType {
        enum NodeItemType {
            // <物品ID> <物品数量> <附加值> [物品组件]
            ITEM_GIVE = 0,
            // <物品ID> <附加值> <物品数量>
            ITEM_CLEAR = 1
        };
    }

    class NodeItem : public NodeBase {
    public:
        const NodeItemType::NodeItemType nodeItemType;

        NodeItem(const std::optional<std::string> &id,
                 const std::optional<std::string> &description,
                 NodeItemType::NodeItemType nodeItemType,
                 const std::vector<std::shared_ptr<CHelper::NamespaceId>> *contents);

        NodeItem(const nlohmann::json &j,
                 const CPack &cpack);

        NODE_TYPE_H;

        void toJson(nlohmann::json &j) const override;

        ASTNode getASTNode(TokenReader &tokenReader, const CPack &cpack) const override;

        std::optional<std::string> getDescription(const ASTNode *node, size_t index) const override;

    };

} // CHelper::Node

CREATE_ADL_SERIALIZER(CHelper::Node::NodeItem);

#endif //CHELPER_NODEITEM_H
