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
    class NodeJsonEntry : public NodeSerializable {
    public:
        std::u16string key;
        std::vector<std::string> value;

    private:
        std::unique_ptr<NodeBase> nodeKey, nodeValue, nodeEntry;

    public:
        NodeJsonEntry() = default;

        NodeJsonEntry(const std::optional<std::string> &id,
                      const std::optional<std::u16string> &description,
                      std::u16string key = std::u16string(),
                      std::vector<std::string> value = std::vector<std::string>());

        [[nodiscard]] NodeTypeId::NodeTypeId getNodeType() const override;

        void init(const std::vector<std::unique_ptr<NodeSerializable>> &dataList);

        ASTNode getASTNode(TokenReader &tokenReader, const CPack *cpack = nullptr) const override;

        static NodeBase *getNodeJsonAllEntry();
    };

}// namespace CHelper::Node

CODEC_NODE(CHelper::Node::NodeJsonEntry, key, value)

CODEC_UNIQUE_PTR(CHelper::Node::NodeJsonEntry)

#endif//CHELPER_NODEJSONENTRY_H
