//
// Created by Yancey on 2024/2/28.
//

#pragma once

#ifndef CHELPER_NODEJSON_H
#define CHELPER_NODEJSON_H

#include "../NodeBase.h"

namespace CHelper::Node {

    //TODO 原始JSON文本的目标选择器要支持*，并且不支持

    class NodeJson : public NodeBase {
    public:
        std::string key;

    private:
        const NodeBase *nodeJson = nullptr;

    public:
        NodeJson(const std::optional<std::string> &id,
                 const std::optional<std::string> &description,
                 std::string key);

        NodeJson() = default;

        void init(const CPack &cpack) override;

        [[nodiscard]] NodeType *getNodeType() const override;

        ASTNode getASTNode(TokenReader &tokenReader, const CPack *cpack) const override;

        void collectStructure(const ASTNode *astNode,
                              StructureBuilder &structure,
                              bool isMustHave) const override;
    };

    CODEC_NODE_H(NodeJson)

}// namespace CHelper::Node

#endif//CHELPER_NODEJSON_H
