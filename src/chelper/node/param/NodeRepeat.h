//
// Created by Yancey on 2024/3/1.
//

#pragma once

#ifndef CHELPER_NODEREPEAT_H
#define CHELPER_NODEREPEAT_H

#include "../../resources/CPack.h"
#include "../NodeBase.h"
#include "../util/NodeOr.h"

namespace CHelper::Node {

    class NodeRepeat : public NodeBase {
    public:
        std::string key;

    private:
        const RepeatData* repeatData = nullptr;
        const NodeBase* nodeElement = nullptr;

    public:
        NodeRepeat() = default;

        [[nodiscard]] NodeType *getNodeType() const override;

        ASTNode getASTNode(TokenReader &tokenReader, const CPack *cpack) const override;

        void collectStructure(const ASTNode *astNode,
                              StructureBuilder &structure,
                              bool isMustHave) const override;
        void init(const CPack &cpack) override;
    };

    CODEC_NODE_H(NodeRepeat)

}// namespace CHelper::Node

#endif//CHELPER_NODEREPEAT_H
