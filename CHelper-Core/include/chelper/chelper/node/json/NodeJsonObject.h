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

        NodeJsonObject(const std::optional<std::u16string> &id,
                       const std::optional<std::u16string> &description);

        void init(const CPack &cpack) override;

        [[nodiscard]] NodeTypeId::NodeTypeId getNodeType() const override;

        ASTNode getASTNode(TokenReader &tokenReader, const CPack *cpack) const override;
    };

}// namespace CHelper::Node

CODEC_NODE(CHelper::Node::NodeJsonObject, data)

#endif//CHELPER_NODEJSONOBJECT_H
