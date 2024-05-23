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
        const std::vector<std::unique_ptr<NodeBase>> data;
        const std::unique_ptr<NodeOr> nodeElement1;
        const NodeOr nodeElement2;
        const NodeList nodeList;

        NodeJsonObject(const std::optional<std::string> &id,
                       const std::optional<std::string> &description,
                       std::vector<std::unique_ptr<NodeBase>> data = {});

        NodeJsonObject(const nlohmann::json &j,
                       [[maybe_unused]] const CPack &cpack);

        NodeJsonObject(BinaryReader &binaryReader,
                       [[maybe_unused]] const CPack &cpack);

        [[nodiscard]] NodeType *getNodeType() const override;

        void toJson(nlohmann::json &j) const override;

        ASTNode getASTNode(TokenReader &tokenReader, const CPack *cpack) const override;
        void writeBinToFile(BinaryWriter &binaryWriter) const override;
    };

}// namespace CHelper::Node

#endif//CHELPER_NODEJSONOBJECT_H
