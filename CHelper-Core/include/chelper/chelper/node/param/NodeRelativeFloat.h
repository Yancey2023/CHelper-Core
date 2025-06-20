//
// Created by Yancey on 2023/12/20.
//

#pragma once

#ifndef CHELPER_NODERELATIVEFLOAT_H
#define CHELPER_NODERELATIVEFLOAT_H

#include "../NodeBase.h"
#include "../util/NodeOr.h"
#include "../util/NodeSingleSymbol.h"

namespace CHelper::Node {

    namespace NodeRelativeFloatType {
        enum NodeRelativeFloatType : uint8_t {
            ABSOLUTE_COORDINATE,
            RELATIVE_WORLD_COORDINATE,
            LOCAL_COORDINATE,
        };
    }

    class NodeRelativeFloat : public NodeSerializable {
    public:
        static std::unique_ptr<NodeSingleSymbol> nodeRelativeNotation;
        static std::unique_ptr<NodeSingleSymbol> nodeCaretNotation;
        static std::unique_ptr<NodeOr> nodePreSymbol;

        bool canUseCaretNotation = true;

        NodeRelativeFloat() = default;

        NodeRelativeFloat(const std::optional<std::string> &id,
                          const std::optional<std::u16string> &description,
                          bool canUseCaretNotation);

        [[nodiscard]] NodeTypeId::NodeTypeId getNodeType() const override;

        ASTNode getASTNode(TokenReader &tokenReader, const CPack *cpack = nullptr) const override;

        bool collectIdError(const ASTNode *astNode,
                            std::vector<std::shared_ptr<ErrorReason>> &idErrorReasons) const override;

        static std::pair<NodeRelativeFloatType::NodeRelativeFloatType, ASTNode>
        getASTNode(const NodeBase *node,
                   const CPack *cpack,
                   TokenReader &tokenReader);
    };

}// namespace CHelper::Node

CODEC_NODE(CHelper::Node::NodeRelativeFloat, canUseCaretNotation)

#endif//CHELPER_NODERELATIVEFLOAT_H
