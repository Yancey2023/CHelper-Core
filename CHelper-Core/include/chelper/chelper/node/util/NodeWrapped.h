//
// Created by Yancey on 2024-12-20.
//

#pragma once

#ifndef CHELPER_NODEWRAPPED_H
#define CHELPER_NODEWRAPPED_H

#include "../NodeBase.h"

namespace CHelper::Node {

    class NodeWrapped : public NodeBase {
    public:
        NodeSerializable *innerNode;
        //存储下一个节点，需要调用构造函数之后再进行添加
        std::vector<NodeWrapped *> nextNodes;

        explicit NodeWrapped(NodeSerializable *innerNode);

        void init(const CPack &cpack) override;

        [[nodiscard]] NodeTypeId::NodeTypeId getNodeType() const override;

        ASTNode getASTNode(TokenReader &tokenReader, const CPack *cpack = nullptr) const override;

        [[nodiscard]] ASTNode getASTNodeWithIsMustAfterSpace(TokenReader &tokenReader, const CPack *cpack, bool isMustAfterSpace) const;

        bool collectIdError(const ASTNode *astNode,
                            std::vector<std::shared_ptr<ErrorReason>> &idErrorReasons) const override;

        bool collectSuggestions(const ASTNode *astNode,
                                size_t index,
                                Suggestions &suggestions) const override;

        void collectStructure(const ASTNode *astNode,
                              StructureBuilder &structure,
                              bool isMustHave) const override;

        bool collectSyntax(const ASTNode *astNode,
                           SyntaxResult &syntaxResult) const override;
};

}

#endif //CHELPER_NODEWRAPPED_H
