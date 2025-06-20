//
// Created by Yancey on 2024-05-25.
//

#ifndef CHELPER_NODEANY_H
#define CHELPER_NODEANY_H

#include "../NodeBase.h"

namespace CHelper::Node {

    class NodeAny : public NodeBase {
    public:
        std::unique_ptr<NodeBase> node;

    private:
        NodeAny() = default;

    public:
        [[nodiscard]] NodeTypeId::NodeTypeId getNodeType() const override;

        void init(const CPack &cpack) override;

        static NodeAny *getNodeAny();
    };

}// namespace CHelper::Node

#endif//CHELPER_NODEANY_H
