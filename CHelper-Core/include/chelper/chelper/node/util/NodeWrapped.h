//
// Created by Yancey on 2024-12-20.
//

#pragma once

#ifndef CHELPER_NODEWRAPPED_H
#define CHELPER_NODEWRAPPED_H

#include "../NodeBase.h"

namespace CHelper::Node {

class NodeWrapped  : public NodeBase {
public:
    NodeBase* innerNode;
    //存储下一个节点，需要调用构造函数之后再进行添加
    std::vector<NodeWrapped *> nextNodes;

    explicit NodeWrapped(NodeBase* innerNode);

    void init(const CPack &cpack) override;

    [[nodiscard]] NodeTypeId::NodeTypeId getNodeType() const override;

    ASTNode getASTNode(TokenReader &tokenReader, const CPack *cpack, void *private_data = nullptr) const override;

    std::optional<std::u16string> collectDescription(const ASTNode *node, size_t index) const override;

    bool collectIdError(const ASTNode *astNode,
                              std::vector<std::shared_ptr<ErrorReason>> &idErrorReasons) const override;

    bool collectSuggestions(const ASTNode *astNode,
                            size_t index,
                            std::vector<Suggestions> &suggestions) const override;

    void collectStructure(const ASTNode *astNode,
                                          StructureBuilder &structure,
                                          bool isMustHave) const override;

    bool collectColor(const ASTNode *astNode,
                      ColoredString &coloredString,
                      const Theme &theme) const override;
};

}

#endif //CHELPER_NODEWRAPPED_H
