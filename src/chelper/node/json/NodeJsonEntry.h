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
        std::string key;
        std::string value;
        std::unique_ptr<NodeBase> nodeKey, nodeEntry;

        NodeJsonEntry(const std::optional<std::string> &id,
                      const std::optional<std::string> &description,
                      std::string key = std::string(),
                      std::string value = std::string());

        explicit NodeJsonEntry(const nlohmann::json &j);

        explicit NodeJsonEntry(BinaryReader &binaryReader);

        void init(const std::vector<std::unique_ptr<NodeBase>> &dataList);

        void toJson(nlohmann::json &j) const override;

        ASTNode getASTNode(TokenReader &tokenReader, const CPack *cpack) const override;

        static NodeBase *getNodeJsonAllEntry();
        void writeBinToFile(BinaryWriter &binaryWriter) const override;
    };

}// namespace CHelper::Node


#endif//CHELPER_NODEJSONENTRY_H
