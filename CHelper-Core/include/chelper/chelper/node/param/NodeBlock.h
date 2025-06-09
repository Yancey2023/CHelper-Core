//
// Created by Yancey on 2023/11/11.
//

#pragma once

#ifndef CHELPER_NODEBLOCK_H
#define CHELPER_NODEBLOCK_H

#include "../NodeBase.h"
#include "NodeNamespaceId.h"

namespace CHelper::Node {

    namespace NodeBlockType {
        enum NodeBlockType : uint8_t {
            // <方块ID> [方块状态]
            BLOCK_WITH_BLOCK_STATE = 0,
            // <方块ID>
            BLOCK = 1
        };
    }// namespace NodeBlockType

    class NodeBlock : public NodeBase {
    public:
        NodeBlockType::NodeBlockType nodeBlockType = NodeBlockType::BLOCK_WITH_BLOCK_STATE;

    private:
        std::shared_ptr<BlockIds> blockIds = nullptr;
        std::shared_ptr<NodeBase> nodeBlockId = nullptr;

    public:
        NodeBlock() = default;

        void init(const CPack &cpack) override;

        [[nodiscard]] NodeTypeId::NodeTypeId getNodeType() const override;

        ASTNode getASTNode(TokenReader &tokenReader, const CPack *cpack = nullptr) const override;

        std::optional<std::u16string> collectDescription(const ASTNode *node, size_t index) const override;

        bool collectSuggestions(const ASTNode *astNode,
                                size_t index,
                                std::vector<Suggestions> &suggestions) const override;

        void collectStructure(const ASTNode *astNode,
                              StructureBuilder &structure,
                              bool isMustHave) const override;
    };

}// namespace CHelper::Node

CODEC_ENUM(CHelper::Node::NodeBlockType::NodeBlockType, uint8_t);

CODEC_NODE(CHelper::Node::NodeBlock, nodeBlockType)

#endif//CHELPER_NODEBLOCK_H
