//
// Created by Yancey on 2024-12-20.
//

#include <chelper/node/NodeType.h>
#include <chelper/node/util/NodeWrapped.h>

namespace CHelper::Node {

    NodeWrapped::NodeWrapped(NodeBase *innerNode)
        : NodeBase("WRAPPED_NODE", u"嵌套节点", false),
          innerNode(innerNode) {}

    void NodeWrapped::init(const CPack &cpack) {
        innerNode->init(cpack);
    }

    NodeTypeId::NodeTypeId NodeWrapped::getNodeType() const {
        return NodeTypeId::WRAPPED;
    }

    ASTNode NodeWrapped::getASTNode(TokenReader &tokenReader, const CPack *cpack) const {
        return getASTNodeWithIsMustAfterWhitespace(tokenReader, cpack, false);
    }

    ASTNode NodeWrapped::getASTNodeWithIsMustAfterWhitespace(TokenReader &tokenReader, const CPack *cpack, bool isMustAfterWhiteSpace) const {
        //空格检测
        bool isMustAfterWhitespace0 = innerNode->getIsMustAfterWhitespace();
        if (innerNode->getNodeType() != NodeTypeId::REPEAT) {
            tokenReader.push();
            if (HEDLEY_UNLIKELY((isMustAfterWhitespace0 || isMustAfterWhiteSpace) && innerNode->getNodeType() != NodeTypeId::LF && tokenReader.skipWhitespace() == 0)) {
                TokensView tokens = tokenReader.collect();
                return ASTNode::simpleNode(this, tokens, ErrorReason::requireWhiteSpace(tokens), ASTNodeId::COMPOUND);
            }
            tokenReader.pop();
        }
        //当前节点
        tokenReader.push();
        DEBUG_GET_NODE_BEGIN(innerNode)
        ASTNode currentASTNode = innerNode->getASTNode(tokenReader, cpack);
        DEBUG_GET_NODE_END(innerNode)
        if (HEDLEY_UNLIKELY(currentASTNode.isError() || nextNodes.empty())) {
            return ASTNode::andNode(this, {std::move(currentASTNode)}, tokenReader.collect(), nullptr, ASTNodeId::COMPOUND);
        }
        //子节点
        std::vector<ASTNode> childASTNodes;
        childASTNodes.reserve(nextNodes.size());
        for (const auto &item: nextNodes) {
            tokenReader.push();
            childASTNodes.push_back(item->getASTNodeWithIsMustAfterWhitespace(tokenReader, cpack, isMustAfterWhitespace0));
            tokenReader.restore();
        }
        tokenReader.push();
        tokenReader.skipToLF();
        ASTNode nextASTNode = ASTNode::orNode(this, std::move(childASTNodes), tokenReader.collect(), nullptr, ASTNodeId::NEXT_NODE);
        return ASTNode::andNode(this, {std::move(currentASTNode), std::move(nextASTNode)}, tokenReader.collect(), nullptr, ASTNodeId::COMPOUND);
    }

    std::optional<std::u16string> NodeWrapped::collectDescription(const ASTNode *node, size_t index) const {
        return innerNode->collectDescription(node, index);
    }

    bool NodeWrapped::collectIdError(const ASTNode *astNode,
                                     std::vector<std::shared_ptr<ErrorReason>> &idErrorReasons) const {
        return innerNode->collectIdError(astNode, idErrorReasons);
    }

    bool NodeWrapped::collectSuggestions(const ASTNode *astNode,
                                         size_t index,
                                         std::vector<Suggestions> &suggestions) const {
        return innerNode->collectSuggestions(astNode, index, suggestions);
    }

    void NodeWrapped::collectStructure(const ASTNode *astNode,
                                       StructureBuilder &structure,
                                       bool isMustHave) const {
        if (astNode == nullptr) {
            innerNode->collectStructure(astNode, structure, isMustHave);
        } else {
            if (HEDLEY_UNLIKELY(brief.has_value())) {
                structure.append(isMustHave, brief.value());
            } else {
                structure.isDirty = false;
                collectStructure(nullptr, structure, isMustHave);
                if (HEDLEY_UNLIKELY(!structure.isDirty)) {
                    structure.appendUnknown(isMustHave);
                    structure.isDirty = false;
                    return;
                }
            }
            if (HEDLEY_UNLIKELY(nextNodes.empty())) {
                return;
            }
            if (HEDLEY_LIKELY(isMustHave)) {
                for (const auto &item: nextNodes) {
                    if (HEDLEY_UNLIKELY(item->innerNode == NodeLF::getInstance())) {
                        isMustHave = false;
                        break;
                    }
                }
            }
            nextNodes[0]->collectStructure(nullptr, structure, isMustHave);
        }
    }

    bool NodeWrapped::collectSyntax(const ASTNode *astNode,
                                    SyntaxResult &syntaxResult) const {
        return innerNode->collectSyntax(astNode, syntaxResult);
    }

}// namespace CHelper::Node