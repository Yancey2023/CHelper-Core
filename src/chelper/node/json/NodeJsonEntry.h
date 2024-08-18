//
// Created by Yancey on 2024/2/28.
//

#pragma once

#ifndef CHELPER_NODEJSONENTRY_H
#define CHELPER_NODEJSONENTRY_H

#include "../NodeBase.h"
#include "../param/NodeText.h"

namespace CHelper::Node {

    //在json数据文件并没有这个类型，添加这个节点类型只是为了更好实现json数据类型中的object类型
    class NodeJsonEntry : public NodeBase {
    public:
        std::wstring key;
        std::vector<std::wstring> value;

    private:
        std::unique_ptr<NodeBase> nodeKey, nodeValue, nodeEntry;

    public:
        NodeJsonEntry() = default;

        NodeJsonEntry(const std::optional<std::wstring> &id,
                      const std::optional<std::wstring> &description,
                      std::wstring key = std::wstring(),
                      std::vector<std::wstring> value = std::vector<std::wstring>());

        [[nodiscard]] NodeType *getNodeType() const override;

        void init(const std::vector<std::unique_ptr<NodeBase>> &dataList);

        ASTNode getASTNode(TokenReader &tokenReader, const CPack *cpack) const override;

        static NodeBase *getNodeJsonAllEntry();
    };

    CODEC_WITH_UNIQUE_PTR_H(NodeJsonEntry)

}// namespace CHelper::Node


#endif//CHELPER_NODEJSONENTRY_H
