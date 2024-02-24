//
// Created by Yancey on 2023/11/11.
//

#ifndef CHELPER_NODEFLOAT_H
#define CHELPER_NODEFLOAT_H

#include "../NodeBase.h"

namespace CHelper::Node {

    class NodeFloat : public NodeBase {
    public:
        std::optional<int> min, max;

        NodeFloat(const std::optional<std::string> &id,
                  const std::optional<std::string> &description,
                  const std::optional<float> &min,
                  const std::optional<float> &max);

        NodeFloat(const nlohmann::json &j,
                  const CPack &cpack);

        [[nodiscard]] NodeType getNodeType() const override;

        void toJson(nlohmann::json &j) const override;

        ASTNode getASTNode(TokenReader &tokenReader, const CPack &cpack) const override;

        void collectStructure(const ASTNode *astNode,
                              StructureBuilder &structure,
                              bool isMustHave) const override;

    };

} // CHelper::Node

#endif //CHELPER_NODEFLOAT_H
