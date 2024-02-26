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
        const std::shared_ptr<NodeBase> nodeItemId;

        NodeItem(const std::optional<std::string> &id,
                 const std::optional<std::string> &description,
                 NodeItemType::NodeItemType nodeItemType,
                 const std::shared_ptr<std::vector<std::shared_ptr<NamespaceId>>> &contents);

        NodeItem(const nlohmann::json &j,
                 const CPack &cpack);

        [[nodiscard]] NodeType getNodeType() const override;

        void toJson(nlohmann::json &j) const override;

        ASTNode getASTNode(TokenReader &tokenReader) const override;

        std::optional<std::string> collectDescription(const ASTNode *node, size_t index) const override;

        void collectStructure(const ASTNode *astNode,
                              StructureBuilder &structure,
                              bool isMustHave) const override;

        static std::shared_ptr<NodeBase>
        getNodeItemId(const std::shared_ptr<std::vector<std::shared_ptr<NamespaceId>>> &contents);

    };

} // CHelper::Node

#endif //CHELPER_NODEITEM_H
