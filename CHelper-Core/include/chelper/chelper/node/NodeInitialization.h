//
// Created by Yancey on 2025-06-20.
//

#pragma once

#ifndef CHELPER_NODEINITIALIZATION_H
#define CHELPER_NODEINITIALIZATION_H

#include <chelper/node/CommandNode.h>
#include <chelper/node/NodeType.h>

namespace CHelper::Node {

    template<class NodeType>
    struct NodeInitialization {
        static void init(NodeType &node, const CPack &cpack) {
        }
    };

    void initNode(Node::NodeWithType node, const CPack &cpack);

}// namespace CHelper::Node

#endif//CHELPER_NODEINITIALIZATION_H
