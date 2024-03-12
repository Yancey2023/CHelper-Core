//
// Created by Yancey on 2023/11/11.
//

#ifndef CHELPER_NODECOMMANDNAME_H
#define CHELPER_NODECOMMANDNAME_H

#include "../NodeBase.h"
#include "../NodeType.h"

namespace CHelper::Node {

    class NodeCommandName : public NodeBase {
    public:
        const std::vector<std::unique_ptr<Node::NodeBase>>* commands;

        NodeCommandName(const std::optional<std::string> &id,
                        const std::optional<std::string> &description,
                        const std::vector<std::unique_ptr<Node::NodeBase>>*commands);

        NodeCommandName(const nlohmann::json &j,
                        const CPack &cpack);

        [[nodiscard]] NodeType* getNodeType() const override;

        ASTNode getASTNode(TokenReader &tokenReader, const CPack *cpack) const override;

        bool collectIdError(const ASTNode *astNode,
                            std::vector<std::shared_ptr<ErrorReason>> &idErrorReasons) const override;

        bool collectSuggestions(const ASTNode *astNode,
                                size_t index,
                                std::vector<Suggestion> &suggestions) const override;

        void collectStructure(const ASTNode *astNode,
                              StructureBuilder &structure,
                              bool isMustHave) const override;

    };

} // CHelper::Node

#endif //CHELPER_NODECOMMANDNAME_H
