//
// Created by Yancey on 2024/2/28.
//

#ifndef CHELPER_NODEJSONFLOAT_H
#define CHELPER_NODEJSONFLOAT_H

#include "../NodeBase.h"

namespace CHelper::Node {

    class NodeJsonFloat : public NodeBase {
    public:
        std::optional<float> min, max;

        NodeJsonFloat(const std::optional<std::string> &id,
                      const std::optional<std::string> &description,
                      const std::optional<float> &min,
                      const std::optional<float> &max);

        NodeJsonFloat(const nlohmann::json &j,
                      [[maybe_unused]] const CPack &cpack);

        [[nodiscard]] NodeType* getNodeType() const override;

        void toJson(nlohmann::json &j) const override;

        ASTNode getASTNode(TokenReader &tokenReader, const CPack *cpack) const override;

        bool collectIdError(const ASTNode *astNode,
                            std::vector<std::shared_ptr<ErrorReason>> &idErrorReasons) const override;

    };


} // CHelper::Node

#endif //CHELPER_NODEJSONFLOAT_H
