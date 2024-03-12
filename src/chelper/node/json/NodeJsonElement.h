//
// Created by Yancey on 2024/2/28.
//

#ifndef CHELPER_NODEJSONELEMENT_H
#define CHELPER_NODEJSONELEMENT_H

#include "../NodeBase.h"

namespace CHelper::Node {

    class NodeJsonElement : public NodeBase {
    public:
        std::vector<std::unique_ptr<NodeBase>> nodes;
        NodeBase* start;

        NodeJsonElement(const std::optional<std::string> &id,
                        const std::optional<std::string> &description,
                        std::vector<std::unique_ptr<NodeBase>> nodes,
                        NodeBase* start);

        NodeJsonElement(const nlohmann::json &j,
                        [[maybe_unused]] const CPack &cpack);

        void toJson(nlohmann::json &j) const override;

        ASTNode getASTNode(TokenReader &tokenReader, const CPack *cpack) const override;

        static NodeBase* getNodeJsonElement();

    };

} // CHelper::Node

#endif //CHELPER_NODEJSONELEMENT_H
