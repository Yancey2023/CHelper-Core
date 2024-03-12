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
        NodeOr nodeCommand;

        NodeCommand(const std::optional<std::string> &id,
                    const std::optional<std::string> &description,
                    const std::vector<const NodeBase*> &childNodes);

        NodeCommand(const nlohmann::json &j,
                    const CPack &cpack);

        [[nodiscard]] NodeType* getNodeType() const override;

        ASTNode getASTNode(TokenReader &tokenReader, const CPack *cpack) const override;

        std::optional<std::string> collectDescription(const ASTNode *node, size_t index) const override;

        void collectStructure(const ASTNode *astNode,
                              StructureBuilder &structure,
                              bool isMustHave) const override;

    };

} // CHelper::Node

#endif //CHELPER_NODECOMMAND_H
