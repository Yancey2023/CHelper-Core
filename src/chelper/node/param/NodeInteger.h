//
// Created by Yancey on 2023/11/11.
//

#ifndef CHELPER_NODEINTEGER_H
#define CHELPER_NODEINTEGER_H

#include "../NodeBase.h"

namespace CHelper::Node {

    class NodeInteger : public NodeBase {
    public:
        std::optional<int> min, max;

        NodeInteger(const std::optional<std::string> &id,
                    const std::optional<std::string> &description,
                    const std::optional<int> &min,
                    const std::optional<int> &max);

        NodeInteger(const nlohmann::json &j,
                    const CPack &cpack);

        [[nodiscard]] NodeType getNodeType() const override;

        void toJson(nlohmann::json &j) const override;

        ASTNode getASTNode(TokenReader &tokenReader) const override;

        void collectStructure(const ASTNode *astNode,
                              StructureBuilder &structure,
                              bool isMustHave) const override;

    };

} // CHelper::Node

#endif //CHELPER_NODEINTEGER_H
