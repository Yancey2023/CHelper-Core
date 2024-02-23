//
// Created by Yancey on 2023/11/11.
//

#ifndef CHELPER_NODEBLOCK_H
#define CHELPER_NODEBLOCK_H

#include "../NodeBase.h"

namespace CHelper::Node {

    namespace NodeBlockType {
        enum NodeBlockType {
            // (已废弃附加值) <方块ID> <附加值 | 方块状态>
            BLOCK_NORMAL = 0,
            // (已废弃) <方块ID> <附加值>
            BLOCK_ONLY_META_DATA = 1
        };
    }

    class NodeBlock : public NodeBase {
    public:
        NodeBlockType::NodeBlockType nodeBlockType;

        NodeBlock(const std::optional<std::string> &id,
                  const std::optional<std::string> &description,
                  NodeBlockType::NodeBlockType nodeBlockType);

        NodeBlock(const nlohmann::json &j,
                  const CPack &cpack);

        NODE_TYPE_H;

        void toJson(nlohmann::json &j) const override;

        ASTNode getASTNode(TokenReader &tokenReader, const CPack &cpack) const override;

        std::optional<std::string> getDescription(const ASTNode *node, size_t index) const override;

    };

} // CHelper::Node

CREATE_ADL_SERIALIZER(CHelper::Node::NodeBlock);

#endif //CHELPER_NODEBLOCK_H
