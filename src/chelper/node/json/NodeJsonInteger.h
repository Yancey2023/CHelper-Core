//
// Created by Yancey on 2024/2/28.
//

#ifndef CHELPER_NODEJSONINTEGER_H
#define CHELPER_NODEJSONINTEGER_H

#include "../NodeBase.h"

namespace CHelper::Node {

    class NodeJsonInteger : public NodeBase {
    public:
        std::optional<int> min, max;

        NodeJsonInteger(const std::optional<std::string> &id,
                    const std::optional<std::string> &description,
                    const std::optional<int> &min,
                    const std::optional<int> &max);

        NodeJsonInteger(const nlohmann::json &j,
                    [[maybe_unused]] const CPack &cpack);

        [[nodiscard]] std::shared_ptr<NodeType> getNodeType() const override;

        void toJson(nlohmann::json &j) const override;

        ASTNode getASTNode(TokenReader &tokenReader) const override;

        bool collectIdError(const ASTNode *astNode,
                            std::vector<std::shared_ptr<ErrorReason>> &idErrorReasons) const override;

    };

} // CHelper::Node

#endif //CHELPER_NODEJSONINTEGER_H
