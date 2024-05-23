//
// Created by Yancey on 2024/2/28.
//

#pragma once

#ifndef CHELPER_NODEJSONINTEGER_H
#define CHELPER_NODEJSONINTEGER_H

#include "../NodeBase.h"

namespace CHelper::Node {

    class NodeJsonInteger : public NodeBase {
    public:
        std::optional<int32_t> min, max;

        NodeJsonInteger(const std::optional<std::string> &id,
                        const std::optional<std::string> &description,
                        const std::optional<int32_t> &min,
                        const std::optional<int32_t> &max);

        NodeJsonInteger(const nlohmann::json &j,
                        [[maybe_unused]] const CPack &cpack);

        NodeJsonInteger(BinaryReader &binaryReader,
                        [[maybe_unused]] const CPack &cpack);

        [[nodiscard]] NodeType *getNodeType() const override;

        void toJson(nlohmann::json &j) const override;

        ASTNode getASTNode(TokenReader &tokenReader, const CPack *cpack) const override;

        bool collectIdError(const ASTNode *astNode,
                            std::vector<std::shared_ptr<ErrorReason>> &idErrorReasons) const override;
        void writeBinToFile(BinaryWriter &binaryWriter) const override;
    };

}// namespace CHelper::Node

#endif//CHELPER_NODEJSONINTEGER_H
