//
// Created by Yancey on 2024/2/28.
//

#pragma once

#ifndef CHELPER_NODEJSONOBJECT_H
#define CHELPER_NODEJSONOBJECT_H

#include "../NodeBase.h"
#include "../util/NodeList.h"
#include "../util/NodeOr.h"
#include "NodeJsonEntry.h"

namespace CHelper::Node {

    class NodeJsonObject : public NodeBase {
    public:
        std::vector<std::unique_ptr<NodeJsonEntry>> data;

    private:
        std::unique_ptr<NodeOr> nodeElement1;
        std::unique_ptr<NodeOr> nodeElement2;
        std::unique_ptr<NodeList> nodeList;

    public:
        NodeJsonObject() = default;

        NodeJsonObject(const std::optional<std::string> &id,
                       const std::optional<std::string> &description);

        void init(const CPack &cpack) override;

        [[nodiscard]] NodeType *getNodeType() const override;

        ASTNode getASTNode(TokenReader &tokenReader, const CPack *cpack) const override;
    };

    CODEC_NODE_H(NodeJsonObject)

}// namespace CHelper::Node

#endif//CHELPER_NODEJSONOBJECT_H
