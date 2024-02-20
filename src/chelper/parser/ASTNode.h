//
// Created by Yancey666 on 2023/12/15.
//

#ifndef CHELPER_ASTNODE_H
#define CHELPER_ASTNODE_H

#include "pch.h"
#include "../lexer/Token.h"
#include "../util/VectorView.h"
#include "ErrorReason.h"
#include "../util/HashUtil.h"

namespace CHelper {

    namespace Node {

        class NodeBase;

    } // Node

    namespace ASTNodeMode {
        enum ASTNodeMode {
            //没有向下的分支
            NONE,
            //有向下的分支，子节点为and关系
            AND,
            //有向下的分支，子节点为or关系
            OR
        };
    }

    class ASTNode {
    public:
        const ASTNodeMode::ASTNodeMode mode;
        const Node::NodeBase *node;
        //子节点为AND类型和OR类型特有
        const std::optional<std::vector<ASTNode>> childNodes;
        const VectorView<Token> tokens;
        const std::vector<std::shared_ptr<ErrorReason>> errorReasons;
        //一个Node可能会生成多个ASTNode，这些ASTNode使用id进行区分
        const std::string id;
        //哪个节点最好，OR类型特有，获取颜色和生成命令格式文本的时候使用
        const int whichBest;

    private:
        ASTNode(ASTNodeMode::ASTNodeMode mode,
                const Node::NodeBase *node,
                const std::vector<ASTNode> &childNodes,
                const VectorView<Token> &tokens,
                const std::vector<std::shared_ptr<ErrorReason>> &errorReasons,
                std::string id,
                int whichBest = -1);

    public:
        static ASTNode simpleNode(const Node::NodeBase *node,
                                  const VectorView<Token> &tokens,
                                  const std::shared_ptr<ErrorReason> &errorReason = nullptr,
                                  const std::string &id = "unknown");

        static ASTNode andNode(const Node::NodeBase *node,
                               const std::vector<ASTNode> &childNodes,
                               const VectorView<Token> &tokens,
                               const std::shared_ptr<ErrorReason> &errorReason = nullptr,
                               const std::string &id = "unknown");

        static ASTNode orNode(const Node::NodeBase *node,
                              const std::vector<ASTNode> &childNodes,
                              const std::optional<VectorView<Token>> &tokens = std::nullopt,
                              const std::shared_ptr<ErrorReason> &errorReason = nullptr,
                              const std::string &id = "unknown");

        [[nodiscard]] bool isError() const;

        [[nodiscard]] bool hasChildNode() const;

    };

} // CHelper

std::ostream &operator<<(std::ostream &os, const CHelper::ASTNode &astNode);

#endif //CHELPER_ASTNODE_H
