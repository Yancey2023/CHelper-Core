//
// Created by Yancey on 2024-05-25.
//

#ifndef CHELPER_NODEANY_H
#define CHELPER_NODEANY_H

#include "../NodeBase.h"

namespace CHelper::Node {

    class NodeAny : public NodeBase {
    private:
        std::unique_ptr<NodeBase> node;

    public:
        NodeAny() = default;

    private:
        NodeAny(const std::optional<std::string> &id,
                const std::optional<std::u16string> &description);

    public:
        [[nodiscard]] NodeTypeId::NodeTypeId getNodeType() const override;

        void init(const CPack &cpack) override;

        ASTNode getASTNode(TokenReader &tokenReader, const CPack *cpack, void *private_data = nullptr) const override;

        static NodeAny *getNodeAny();
    };

}// namespace CHelper::Node

#endif//CHELPER_NODEANY_H
