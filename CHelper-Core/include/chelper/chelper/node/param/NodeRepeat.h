//
// Created by Yancey on 2024/3/1.
//

#pragma once

#ifndef CHELPER_NODEREPEAT_H
#define CHELPER_NODEREPEAT_H

#include "../../resources/CPack.h"
#include "../NodeBase.h"

namespace CHelper::Node {

    class NodeRepeat : public NodeSerializable {
    public:
        std::string key;

    private:
        const RepeatData *repeatData = nullptr;
        const NodeBase *nodeElement = nullptr;

    public:
        NodeRepeat() = default;

        void init(const CPack &cpack) override;

        [[nodiscard]] NodeTypeId::NodeTypeId getNodeType() const override;

        ASTNode getASTNode(TokenReader &tokenReader, const CPack *cpack = nullptr) const override;

        std::optional<std::u16string> collectDescription(const ASTNode *node, size_t index) const override;

        void collectStructure(const ASTNode *astNode,
                              StructureBuilder &structure,
                              bool isMustHave) const override;
    };

}// namespace CHelper::Node

CODEC_NODE(CHelper::Node::NodeRepeat, key)

#endif//CHELPER_NODEREPEAT_H
