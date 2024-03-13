//
// Created by Yancey on 2024/2/28.
//

#ifndef CHELPER_NODEJSONNULL_H
#define CHELPER_NODEJSONNULL_H

#include "../NodeBase.h"

namespace CHelper::Node {

    class NodeJsonNull : public NodeBase {
    public:
        NodeJsonNull(const std::optional<std::string> &id,
                     const std::optional<std::string> &description);

        NodeJsonNull(const nlohmann::json &j,
                     [[maybe_unused]]const CPack &cpack);

        [[nodiscard]] NodeType* getNodeType() const override;

        ASTNode getASTNode(TokenReader &tokenReader, const CPack *cpack) const override;

        bool collectSuggestions(const ASTNode *astNode,
                                size_t index,
                                std::vector<Suggestions> &suggestions) const override;

    };

}

#endif //CHELPER_NODEJSONNULL_H
