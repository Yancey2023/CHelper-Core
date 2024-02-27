//
// Created by Yancey on 2023/11/11.
//

#ifndef CHELPER_NODEBOOLEAN_H
#define CHELPER_NODEBOOLEAN_H

#include "../NodeBase.h"

namespace CHelper::Node {

    class NodeBoolean : public NodeBase {
    public:
        std::optional<std::string> descriptionTrue, descriptionFalse;

        NodeBoolean(const std::optional<std::string> &id,
                    const std::optional<std::string> &description,
                    const std::optional<std::string> &descriptionTrue,
                    const std::optional<std::string> &descriptionFalse);

        NodeBoolean(const nlohmann::json &j,
                    [[maybe_unused]] const CPack &cpack);

        [[nodiscard]] NodeType getNodeType() const override;

        void toJson(nlohmann::json &j) const override;

        ASTNode getASTNode(TokenReader &tokenReader) const override;

        void collectStructure(const ASTNode *astNode,
                              StructureBuilder &structure,
                              bool isMustHave) const override;

    };

} // CHelper::Node

#endif //CHELPER_NODEBOOLEAN_H
