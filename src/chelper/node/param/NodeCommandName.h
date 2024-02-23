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
        NodeCommandName(const std::optional<std::string> &id,
                        const std::optional<std::string> &description);

        NodeCommandName(const nlohmann::json &j,
                        const CPack &cpack);

        NODE_TYPE_H;

        ASTNode getASTNode(TokenReader &tokenReader, const CPack &cpack) const override;

        bool collectIdError(const ASTNode *astNode,
                            const CPack &cpack,
                            std::vector<std::shared_ptr<ErrorReason>> &idErrorReasons) const override;

        bool collectSuggestions(const ASTNode *astNode,
                                const CPack &cpack,
                                std::vector<Suggestion> &suggestions) const override;

        void collectStructure(const ASTNode *astNode,
                              StructureBuilder &structure,
                              bool isMustHave) const override;

    };

} // CHelper::Node

CREATE_ADL_SERIALIZER(CHelper::Node::NodeCommandName);

#endif //CHELPER_NODECOMMANDNAME_H
