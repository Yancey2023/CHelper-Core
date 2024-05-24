//
// Created by Yancey on 2024/2/28.
//

#pragma once

#ifndef CHELPER_NODEJSONELEMENT_H
#define CHELPER_NODEJSONELEMENT_H

#include "../NodeBase.h"

namespace CHelper::Node {

    class NodeJsonElement : public NodeBase {
    public:
        std::vector<std::unique_ptr<NodeBase>> nodes;
        std::string startNodeId;

    private:
        NodeBase *start = nullptr;

    public:
        void init(const CPack &cpack) override;

        ASTNode getASTNode(TokenReader &tokenReader, const CPack *cpack) const override;

        static NodeBase *getNodeJsonElement();
    };

    CODEC_H(std::unique_ptr<NodeJsonElement>)

}// namespace CHelper::Node

#endif//CHELPER_NODEJSONELEMENT_H
