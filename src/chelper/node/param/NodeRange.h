//
// Created by Yancey on 2024/2/27.
//

#ifndef CHELPER_NODERANGE_H
#define CHELPER_NODERANGE_H

#include "../NodeBase.h"

namespace CHelper::Node {

    class NodeRange : public NodeBase {
    public:
        NodeRange(const std::optional<std::string> &id,
                  const std::optional<std::string> &description);

        NodeRange(const nlohmann::json &j,
                  [[maybe_unused]] const CPack &cpack);

        [[nodiscard]] NodeType getNodeType() const override;

        ASTNode getASTNode(TokenReader &tokenReader) const override;

        bool collectSuggestions(const ASTNode *astNode,
                                size_t index,
                                std::vector<Suggestion> &suggestions) const override;

        void collectStructure(const ASTNode *astNode,
                              StructureBuilder &structure,
                              bool isMustHave) const override;

    };

} // CHelper::Node

#endif //CHELPER_NODERANGE_H
