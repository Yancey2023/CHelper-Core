//
// Created by Yancey on 2023/12/20.
//

#pragma once

#ifndef CHELPER_NODERELATIVEFLOAT_H
#define CHELPER_NODERELATIVEFLOAT_H

#include "../NodeBase.h"

namespace CHelper::Node {

    class NodeRelativeFloat : public NodeBase {
    public:
        bool canUseCaretNotation;

        NodeRelativeFloat(const std::optional<std::string> &id,
                          const std::optional<std::string> &description,
                          bool canUseCaretNotation);

        NodeRelativeFloat(const nlohmann::json &j,
                          [[maybe_unused]] const CPack &cpack);

        NodeRelativeFloat(BinaryReader &binaryReader,
                          [[maybe_unused]] const CPack &cpack);

        [[nodiscard]] NodeType *getNodeType() const override;

        void toJson(nlohmann::json &j) const override;

        ASTNode getASTNode(TokenReader &tokenReader, const CPack *cpack) const override;

        static std::pair<uint8_t, ASTNode> getASTNode(const NodeBase *node,
                                                           const CPack *cpack,
                                                           TokenReader &tokenReader);

        void collectStructure(const ASTNode *astNode,
                              StructureBuilder &structure,
                              bool isMustHave) const override;
        void writeBinToFile(BinaryWriter &binaryWriter) const override;
    };

}// namespace CHelper::Node

#endif//CHELPER_NODERELATIVEFLOAT_H
