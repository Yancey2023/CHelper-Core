//
// Created by Yancey on 2023/11/10.
//

#include <chelper/node/NodeBase.h>

#ifdef CHelperDebug
#include <chelper/node/NodeType.h>
#endif

namespace CHelper::Node {

    NodeBase::NodeBase(const std::optional<std::string> &id,
                       const std::optional<std::u16string> &description,
                       bool isMustAfterWhiteSpace)
        : id(id),
          description(description),
          isMustAfterWhiteSpace(isMustAfterWhiteSpace) {}

    void NodeBase::init(const CPack &cpack) {
    }

    ASTNode NodeBase::getByChildNode(TokenReader &tokenReader,
                                     const CPack *cpack,
                                     const NodeBase *childNode,
                                     const ASTNodeId::ASTNodeId &astNodeId) const {
        ASTNode node = childNode->getASTNode(tokenReader, cpack);
        TokensView tokens = node.tokens;
        return ASTNode::andNode(this, {std::move(node)}, tokens, nullptr, astNodeId);
    }

    /**
     * 节点不一定有
     *
     * @param tokenReader token读取器
     * @param cpack 资源包
     * @param isIgnoreChildNodesError true - 第一个错误节点到后面都不算做子节点
     *                                false - 第一个内容为空的错误节点到后面都不算做子节点
     * @param childNodes 子节点
     * @param astNodeId 节点ID
     */
    ASTNode NodeBase::getOptionalASTNode(TokenReader &tokenReader,
                                         const CPack *cpack,
                                         bool isIgnoreChildNodesError,
                                         const std::vector<const NodeBase *> &childNodes,
                                         const ASTNodeId::ASTNodeId &astNodeId) const {
        tokenReader.push();
        std::vector<ASTNode> childASTNodes;
        for (const auto &item: childNodes) {
            tokenReader.push();
            tokenReader.push();
            DEBUG_GET_NODE_BEGIN(item)
            ASTNode astNode = item->getASTNode(tokenReader, cpack);
            DEBUG_GET_NODE_END(item)
            bool isError = astNode.isError();
            const TokensView tokens = tokenReader.collect();
            if (HEDLEY_UNLIKELY(isError && (isIgnoreChildNodesError || tokens.isEmpty()))) {
                tokenReader.restore();
                break;
            }
            childASTNodes.push_back(std::move(astNode));
            tokenReader.pop();
            if (HEDLEY_UNLIKELY(isError)) {
                break;
            }
        }
        return ASTNode::andNode(this, std::move(childASTNodes), tokenReader.collect(), nullptr, astNodeId);
    }

    std::optional<std::u16string> NodeBase::collectDescription(const ASTNode *node, size_t index) const {
#ifdef CHelperDebug
        if (HEDLEY_UNLIKELY(!description.has_value())) {
            SPDLOG_WARN("description is null");
        }
#endif
        return description;
    }

    //创建AST节点的时候只得到了结构的错误，ID的错误需要调用这个方法得到
    bool NodeBase::collectIdError(const ASTNode *astNode,
                                  std::vector<std::shared_ptr<ErrorReason>> &idErrorReasons) const {
        return false;
    }

    bool NodeBase::collectSuggestions(const ASTNode *astNode,
                                      size_t index,
                                      std::vector<Suggestions> &suggestions) const {
        return false;
    }

    void NodeBase::collectStructure(const ASTNode *astNode,
                                    StructureBuilder &structure,
                                    bool isMustHave) const {
    }

    bool NodeBase::collectSyntax(const ASTNode *astNode,
                                 SyntaxResult &syntaxResult) const {
        return false;
    }

    bool NodeBase::isAfterWhitespace() const {
        return isMustAfterWhiteSpace.value_or(false);
    }

}// namespace CHelper::Node
