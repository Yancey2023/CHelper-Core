//
// Created by Yancey on 2024/2/28.
//

#pragma once

#ifndef CHELPER_NODEJSONSTRING_H
#define CHELPER_NODEJSONSTRING_H

#include "../NodeBase.h"

namespace CHelper::Node {

    class NodeJsonString : public NodeSerializable {
    public:
        std::optional<std::vector<std::unique_ptr<NodeSerializable>>> data;

    private:
        std::unique_ptr<NodeBase> nodeData;

    public:
        NodeJsonString() = default;

        NodeJsonString(const std::optional<std::string> &id,
                       const std::optional<std::u16string> &description);

        void init(const CPack &cpack) override;

        [[nodiscard]] NodeTypeId::NodeTypeId getNodeType() const override;

        ASTNode getASTNode(TokenReader &tokenReader, const CPack *cpack = nullptr) const override;

        bool collectIdError(const ASTNode *astNode,
                            std::vector<std::shared_ptr<ErrorReason>> &idErrorReasons) const override;
    };

}// namespace CHelper::Node

CODEC_NODE(CHelper::Node::NodeJsonString, data)

#endif//CHELPER_NODEJSONSTRING_H
