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

        NodeAny(const std::optional<std::string> &id,
                const std::optional<std::string> &description);

    public:
        ASTNode getASTNode(TokenReader &tokenReader, const CPack *cpack) const override;

        static NodeAny *getNodeAny();
        void init(const CPack &cpack) override;
    };

}// namespace CHelper::Node

#endif//CHELPER_NODEANY_H
