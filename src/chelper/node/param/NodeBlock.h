//
// Created by Yancey on 2023/11/11.
//

#ifndef CHELPER_NODEBLOCK_H
#define CHELPER_NODEBLOCK_H

#include "../NodeBase.h"
#include "NodeNamespaceId.h"

namespace CHelper::Node {

    namespace NodeBlockType {
        enum NodeBlockType : std::uint8_t {
            // <方块ID> <方块状态>
            BLOCK_WITH_BLOCK_STATE = 0,
            // <方块ID>
            BLOCK = 1
        };
    }

    class NodeBlock : public NodeBase {
    public:
        const NodeBlockType::NodeBlockType nodeBlockType;
        const std::shared_ptr<std::vector<std::shared_ptr<NamespaceId>>> blockIds;
        const std::shared_ptr<NodeBase> nodeBlockId;

        NodeBlock(const std::optional<std::string> &id,
                  const std::optional<std::string> &description,
                  NodeBlockType::NodeBlockType nodeBlockType,
                  const std::shared_ptr<std::vector<std::shared_ptr<NamespaceId>>> &contents);

        NodeBlock(const nlohmann::json &j,
                  const CPack &cpack);

        [[nodiscard]] std::shared_ptr<NodeType> getNodeType() const override;

        void toJson(nlohmann::json &j) const override;

        ASTNode getASTNode(TokenReader &tokenReader, const CPack &cpack) const override;

        std::optional<std::string> collectDescription(const ASTNode *node, size_t index) const override;

        void collectStructure(const ASTNode *astNode,
                              StructureBuilder &structure,
                              bool isMustHave) const override;

    };

} // CHelper::Node

#endif //CHELPER_NODEBLOCK_H
