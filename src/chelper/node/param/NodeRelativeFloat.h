//
// Created by Yancey666 on 2023/12/20.
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
                          const CPack &cpack);

        NODE_TYPE_H;

        void toJson(nlohmann::json &j) const override;

        ASTNode getASTNode(TokenReader &tokenReader, const CPack &cpack) const override;

        static std::pair<int, ASTNode> getASTNode(const NodeBase *node,
                                           TokenReader &tokenReader,
                                           const CPack &cpack,
                                           bool canUseCaretNotation);
    };

} // CHelper::Node

#endif //CHELPER_NODERELATIVEFLOAT_H
