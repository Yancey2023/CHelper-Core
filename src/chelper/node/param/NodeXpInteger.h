//
// Created by Yancey on 2024/3/1.
//

#ifndef CHELPER_NODEXPINTEGER_H
#define CHELPER_NODEXPINTEGER_H

#include "../NodeBase.h"

namespace CHelper::Node {

    class NodeXpInteger : public NodeBase {
    public:
        NodeXpInteger(const std::optional<std::string> &id,
                      const std::optional<std::string> &description);

        NodeXpInteger(const nlohmann::json &j,
                      [[maybe_unused]] const CPack &cpack);

        [[nodiscard]] std::shared_ptr<NodeType> getNodeType() const override;

        ASTNode getASTNode(TokenReader &tokenReader, const CPack &cpack) const override;

        void collectStructure(const ASTNode *astNode,
                              StructureBuilder &structure,
                              bool isMustHave) const override;

    };

} // CHelper::Node

#endif //CHELPER_NODEXPINTEGER_H
