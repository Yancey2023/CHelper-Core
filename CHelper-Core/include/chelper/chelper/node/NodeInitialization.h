//
// Created by Yancey on 2025-06-20.
//

#pragma once

#ifndef CHELPER_NODEINITIALIZATION_H
#define CHELPER_NODEINITIALIZATION_H

#include <chelper/node/CommandNode.h>
#include <chelper/node/NodeType.h>

namespace CHelper::Node {

    template<NodeTypeId::NodeTypeId nodeTypeId>
    struct NodeInitialization {
        using NodeType = typename NodeTypeDetail<nodeTypeId>::Type;
        static_assert(std::is_base_of_v<Node::NodeBase, NodeType>, "NodeType must be derived from NodeBase");
        static void init(NodeType &node, const CPack &cpack) {
        }
    };

    void initNode(Node::NodeBase &node, const CPack &cpack);

}// namespace CHelper::Node

#endif//CHELPER_NODEINITIALIZATION_H
