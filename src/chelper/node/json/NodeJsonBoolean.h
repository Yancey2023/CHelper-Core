//
// Created by Yancey on 2024/2/28.
//

#ifndef CHELPER_NODEJSONBOOLEAN_H
#define CHELPER_NODEJSONBOOLEAN_H

#include "../NodeBase.h"

namespace CHelper::Node {

    class NodeJsonBoolean : public NodeBase {
    public:
        std::optional<std::string> descriptionTrue, descriptionFalse;

        NodeJsonBoolean(const std::optional<std::string> &id,
                        const std::optional<std::string> &description,
                        const std::optional<std::string> &descriptionTrue,
                        const std::optional<std::string> &descriptionFalse);

        NodeJsonBoolean(const nlohmann::json &j,
                        [[maybe_unused]] const CPack &cpack);

        [[nodiscard]] NodeType* getNodeType() const override;

        void toJson(nlohmann::json &j) const override;

        ASTNode getASTNode(TokenReader &tokenReader, const CPack *cpack) const override;

    };

} // CHelper::Node

#endif //CHELPER_NODEJSONBOOLEAN_H
