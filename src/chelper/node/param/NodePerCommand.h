//
// Created by Yancey on 2023/11/10.
//

#ifndef CHELPER_NODEPERCOMMAND_H
#define CHELPER_NODEPERCOMMAND_H

#include "pch.h"
#include "../NodeBase.h"
#include "../util/NodeOr.h"

namespace CHelper::Node {

    class NodePerCommand : public NodeBase {
    public:
        std::vector<std::string> name;
        std::vector<std::shared_ptr<Node::NodeBase>> nodes;
        std::vector<std::shared_ptr<Node::NodeBase>> startNodes;

    public:
        NodePerCommand(const std::optional<std::string> &id,
                       std::vector<std::string> name,
                       const std::optional<std::string> &description,
                       std::vector<std::shared_ptr<Node::NodeBase>> nodes,
                       std::vector<std::shared_ptr<Node::NodeBase>> startNodes);

        NodePerCommand(const nlohmann::json &j,
                       const CPack &cpack);

        ~NodePerCommand();

        [[nodiscard]] NodeType getNodeType() const override;

        void toJson(nlohmann::json &j) const override;

        ASTNode getASTNode(TokenReader &tokenReader) const override;

        std::optional<std::string> collectDescription(const ASTNode *pNode, size_t index) const override;

        bool collectSuggestions(const ASTNode *astNode,
                                size_t index,
                                std::vector<Suggestion> &suggestions) const override;

        void collectStructure(const ASTNode *astNode,
                              StructureBuilder &structure,
                              bool isMustHave) const override;

    }; // NodePerCommand::Node

} // CHelper

#endif //CHELPER_NODEPERCOMMAND_H
