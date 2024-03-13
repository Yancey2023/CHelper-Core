//
// Created by Yancey on 2024/3/1.
//

#ifndef CHELPER_NODEREPEAT_H
#define CHELPER_NODEREPEAT_H

#include "../NodeBase.h"

namespace CHelper::Node {

    class NodeRepeat : public NodeBase {
    public:
        std::string key;
        std::pair<NodeBase *, NodeBase *> node;

        NodeRepeat(const std::optional<std::string> &id,
                   const std::optional<std::string> &description,
                   std::string key,
                   const std::pair<NodeBase *, NodeBase *> &node);

        NodeRepeat(const nlohmann::json &j,
                   const CPack &cpack);

        [[nodiscard]] NodeType *getNodeType() const override;

        void toJson(nlohmann::json &j) const override;

        ASTNode getASTNode(TokenReader &tokenReader, const CPack *cpack) const override;

        void collectStructure(const ASTNode *astNode,
                              StructureBuilder &structure,
                              bool isMustHave) const override;

    };

} // CHelper::Node

#endif //CHELPER_NODEREPEAT_H
