//
// Created by Yancey on 2024/3/1.
//

#pragma once

#ifndef CHELPER_NODEREPEAT_H
#define CHELPER_NODEREPEAT_H

#include "../NodeBase.h"
#include "../util/NodeOr.h"

namespace CHelper::Node {

    class NodeRepeat : public NodeBase {
    public:
        std::string key;

    private:
        std::unique_ptr<NodeOr> nodeElement;

    public:
        NodeRepeat() = default;

        NodeRepeat(const std::optional<std::string> &id,
                   const std::optional<std::string> &description,
                   std::string key,
                   const std::pair<NodeBase *, NodeBase *> &node);

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
