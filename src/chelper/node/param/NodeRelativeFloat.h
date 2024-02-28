//
// Created by Yancey on 2023/12/20.
//

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

        [[nodiscard]] std::shared_ptr<NodeType> getNodeType() const override;

        void toJson(nlohmann::json &j) const override;

        ASTNode getASTNode(TokenReader &tokenReader) const override;

        static std::pair<int, ASTNode> getASTNode(const NodeBase *node,
                                                  TokenReader &tokenReader,
                                                  bool canUseCaretNotation);

        void collectStructure(const ASTNode *astNode,
                              StructureBuilder &structure,
                              bool isMustHave) const override;

    };

} // CHelper::Node

#endif //CHELPER_NODERELATIVEFLOAT_H
