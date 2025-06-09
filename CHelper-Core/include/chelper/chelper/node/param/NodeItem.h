//
// Created by Yancey on 2023/11/11.
//

#pragma once

#ifndef CHELPER_NODEITEM_H
#define CHELPER_NODEITEM_H

#include "../NodeBase.h"
#include "NodeNamespaceId.h"

namespace CHelper::Node {

    namespace NodeItemType {
        enum NodeItemType : uint8_t {
            // <物品ID> <物品数量> <附加值> [物品组件]
            ITEM_GIVE = 0,
            // <物品ID> <附加值> <物品数量>
            ITEM_CLEAR = 1
        };
    }// namespace NodeItemType

    class NodeItem : public NodeSerializable {
    public:
        NodeItemType::NodeItemType nodeItemType = NodeItemType::ITEM_GIVE;

    private:
        std::unique_ptr<NodeNamespaceId> nodeItemId;
        std::shared_ptr<std::vector<std::shared_ptr<ItemId>>> itemIds;
        std::unique_ptr<NodeBase> nodeComponent;

    public:
        NodeItem() = default;

        void init(const CPack &cpack) override;

        [[nodiscard]] NodeTypeId::NodeTypeId getNodeType() const override;

        ASTNode getASTNode(TokenReader &tokenReader, const CPack *cpack = nullptr) const override;

        std::optional<std::u16string> collectDescription(const ASTNode *node, size_t index) const override;

        void collectStructure(const ASTNode *astNode,
                              StructureBuilder &structure,
                              bool isMustHave) const override;

        ASTNode getOptionalASTNode(TokenReader &tokenReader,
                                   const CPack *cpack,
                                   bool isIgnoreChildNodesError,
                                   const std::vector<const NodeBase *> &childNodes,
                                   const ASTNodeId::ASTNodeId &astNodeId = ASTNodeId::NONE) const;
    };

}// namespace CHelper::Node

CODEC_ENUM(CHelper::Node::NodeItemType::NodeItemType, uint8_t);

CODEC_NODE(CHelper::Node::NodeItem, nodeItemType)

#endif//CHELPER_NODEITEM_H
