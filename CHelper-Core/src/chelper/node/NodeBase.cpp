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

    bool NodeBase::getIsMustAfterWhitespace() const {
        return isMustAfterWhiteSpace.value_or(false);
    }

}// namespace CHelper::Node
