//
// Created by Yancey on 2024/2/28.
//

#pragma once

#ifndef CHELPER_NODEJSONSTRING_H
#define CHELPER_NODEJSONSTRING_H

#include "../NodeBase.h"

namespace CHelper::Node {

    class NodeJsonString : public NodeBase {
    public:
        std::optional<std::vector<std::unique_ptr<NodeBase>>> data;

    private:
        std::unique_ptr<NodeBase> nodeData;

    public:
        NodeJsonString() = default;

        NodeJsonString(const std::optional<std::wstring> &id,
                       const std::optional<std::wstring> &description);

        void init(const CPack &cpack) override;

        [[nodiscard]] NodeType *getNodeType() const override;

        ASTNode getASTNode(TokenReader &tokenReader, const CPack *cpack) const override;

        bool collectIdError(const ASTNode *astNode,
                            std::vector<std::shared_ptr<ErrorReason>> &idErrorReasons) const override;

        bool collectSuggestions(const ASTNode *astNode,
                                size_t index,
                                std::vector<Suggestions> &suggestions) const override;

        bool collectColor(const ASTNode *astNode,
                          ColoredString &coloredString,
                          const Theme &theme) const override;
    };

    CODEC_NODE_H(NodeJsonString)

}// namespace CHelper::Node

#endif//CHELPER_NODEJSONSTRING_H
