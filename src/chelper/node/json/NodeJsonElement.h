//
// Created by Yancey on 2024/2/28.
//

#ifndef CHELPER_NODEJSONELEMENT_H
#define CHELPER_NODEJSONELEMENT_H

#include "../NodeBase.h"

namespace CHelper::Node {

    class NodeJsonElement : public NodeBase {
    public:
        std::vector<std::shared_ptr<NodeBase>> nodes;
        std::shared_ptr<NodeBase> start;

        NodeJsonElement(const std::optional<std::string> &id,
                        const std::optional<std::string> &description,
                        const std::vector<std::shared_ptr<NodeBase>> &nodes);

        NodeJsonElement(const nlohmann::json &j,
                        [[maybe_unused]] const CPack &cpack);

        ~NodeJsonElement();

        void toJson(nlohmann::json &j) const override;

        ASTNode getASTNode(TokenReader &tokenReader, const CPack &cpack) const override;

        static std::shared_ptr<NodeBase> getNodeJsonElement();

    };

} // CHelper::Node

#endif //CHELPER_NODEJSONELEMENT_H
