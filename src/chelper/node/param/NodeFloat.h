//
// Created by Yancey on 2023/11/11.
//

#pragma once

#ifndef CHELPER_NODEFLOAT_H
#define CHELPER_NODEFLOAT_H

#include "../NodeBase.h"

namespace CHelper::Node {

    class NodeFloat : public NodeBase {
    public:
        std::optional<std::float_t> min, max;

        NodeFloat(const std::optional<std::string> &id,
                  const std::optional<std::string> &description,
                  const std::optional<std::float_t> &min,
                  const std::optional<std::float_t> &max);

        NodeFloat(const nlohmann::json &j,
                  [[maybe_unused]] const CPack &cpack);

        [[nodiscard]] NodeType *getNodeType() const override;

        void toJson(nlohmann::json &j) const override;

        ASTNode getASTNode(TokenReader &tokenReader, const CPack *cpack) const override;

        bool collectIdError(const ASTNode *astNode,
                            std::vector<std::shared_ptr<ErrorReason>> &idErrorReasons) const override;

        void collectStructure(const ASTNode *astNode,
                              StructureBuilder &structure,
                              bool isMustHave) const override;

    };

} // CHelper::Node

#endif //CHELPER_NODEFLOAT_H
