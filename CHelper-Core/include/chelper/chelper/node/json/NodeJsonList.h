//
// Created by Yancey on 2024/2/28.
//

#pragma once

#ifndef CHELPER_NODEJSONLIST_H
#define CHELPER_NODEJSONLIST_H

#include "../NodeBase.h"

namespace CHelper::Node {

    class NodeJsonList : public NodeBase {
    public:
        std::u16string data;

    private:
        std::unique_ptr<NodeBase> nodeList;

    public:
        NodeJsonList() = default;

        NodeJsonList(const std::optional<std::u16string> &id,
                     const std::optional<std::u16string> &description,
                     std::u16string data = std::u16string());

        void init(const std::vector<std::unique_ptr<NodeBase>> &dataList);

        [[nodiscard]] NodeType *getNodeType() const override;

        ASTNode getASTNode(TokenReader &tokenReader, const CPack *cpack) const override;

        bool collectSuggestions(const ASTNode *astNode,
                                size_t index,
                                std::vector<Suggestions> &suggestions) const override;
    };

    CODEC_NODE_H(NodeJsonList)

}// namespace CHelper::Node

#endif//CHELPER_NODEJSONLIST_H
