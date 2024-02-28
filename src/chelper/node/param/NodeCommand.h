//
// Created by Yancey on 2023/11/11.
//

#ifndef CHELPER_NODECOMMAND_H
#define CHELPER_NODECOMMAND_H

#include "../NodeBase.h"
#include "../util/NodeOr.h"

namespace CHelper::Node {

    class NodeCommand : public NodeBase {
    public:
        std::shared_ptr<NodeOr> nodeCommand;

        NodeCommand(const std::optional<std::string> &id,
                    const std::optional<std::string> &description,
                    const std::shared_ptr<std::vector<std::shared_ptr<NodeBase>>> &childNodes);

        NodeCommand(const nlohmann::json &j,
                    const CPack &cpack);

        [[nodiscard]] std::shared_ptr<NodeType> getNodeType() const override;

        ASTNode getASTNode(TokenReader &tokenReader) const override;

        std::optional<std::string> collectDescription(const ASTNode *node, size_t index) const override;

        void collectStructure(const ASTNode *astNode,
                              StructureBuilder &structure,
                              bool isMustHave) const override;

    };

} // CHelper::Node

#endif //CHELPER_NODECOMMAND_H
