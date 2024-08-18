//
// Created by Yancey on 2023/12/23.
//

#include "NodeList.h"

namespace CHelper::Node {

    NodeList::NodeList(const std::optional<std::wstring> &id,
                       const std::optional<std::wstring> &description,
                       NodeBase *nodeLeft,
                       NodeBase *nodeElement,
                       NodeBase *nodeSeparator,
                       NodeBase *nodeRight)
        : NodeBase(id, description, false),
          nodeLeft(nodeLeft),
          nodeElement(nodeElement),
          nodeSeparator(nodeSeparator),
          nodeRight(nodeRight),
          nodeElementOrRight(
                  L"ELEMENT_OR_RIGHT", L"element or right",
                  std::vector<const NodeBase *>{
                          nodeElement, nodeRight},
                  false),
          nodeSeparatorOrRight(
                  L"SEPARATOR_OR_RIGHT", L"separator or right",
                  std::vector<const NodeBase *>{
                          nodeSeparator, nodeRight},
                  false) {
#if CHelperDebug == true
        if (HEDLEY_UNLIKELY(
                    nodeLeft == nullptr || nodeElement == nullptr || nodeSeparator == nullptr || nodeRight == nullptr)) {
            throw std::runtime_error("NodeOr has a null child node");
        }
#endif
    }

    NodeType *NodeList::getNodeType() const {
        return NodeType::LIST.get();
    }

    ASTNode NodeList::getASTNode(TokenReader &tokenReader, const CPack *cpack) const {
        //标记整个[...]，在最后进行收集
        tokenReader.push();
        ASTNode left = nodeLeft->getASTNodeWithNextNode(tokenReader, cpack);
        if (HEDLEY_UNLIKELY(left.isError())) {
            return ASTNode::andNode(this, {std::move(left)}, tokenReader.collect());
        }
        std::vector<ASTNode> childNodes = {std::move(left)};
        {
#if CHelperDebug == true
            size_t startIndex = tokenReader.index;
#endif
            //检测[]中间有没有内容
            tokenReader.push();
            DEBUG_GET_NODE_BEGIN(nodeRight)
            auto rightBracket1 = nodeRight->getASTNodeWithNextNode(tokenReader, cpack);
            DEBUG_GET_NODE_END(nodeRight)
            tokenReader.restore();
            auto elementOrRight = nodeElementOrRight.getASTNodeWithNextNode(tokenReader, cpack);
            bool flag = !rightBracket1.isError() || elementOrRight.isError();
            childNodes.push_back(std::move(elementOrRight));
            if (HEDLEY_UNLIKELY(flag)) {
                return ASTNode::andNode(this, std::move(childNodes), tokenReader.collect());
            }
#if CHelperDebug == true
            if (HEDLEY_UNLIKELY(startIndex == tokenReader.index)) {
                CHELPER_WARN(L"NodeList has some error");
                return ASTNode::andNode(this, std::move(childNodes), tokenReader.collect());
            }
#endif
        }
        while (true) {
#if CHelperDebug == true
            size_t startIndex = tokenReader.index;
#endif
            //检测是分隔符还是右括号
            tokenReader.push();
            DEBUG_GET_NODE_BEGIN(nodeRight)
            auto rightBracket = nodeRight->getASTNodeWithNextNode(tokenReader, cpack);
            DEBUG_GET_NODE_END(nodeRight)
            tokenReader.restore();
            DEBUG_GET_NODE_BEGIN(nodeSeparator)
            ASTNode separatorOrRight = nodeSeparatorOrRight.getASTNodeWithNextNode(tokenReader, cpack);
            DEBUG_GET_NODE_END(nodeSeparator)
            bool flag = !rightBracket.isError() || separatorOrRight.isError();
            childNodes.push_back(std::move(separatorOrRight));
            if (HEDLEY_UNLIKELY(flag)) {
                return ASTNode::andNode(this, std::move(childNodes), tokenReader.collect());
            }
            //检测是不是元素
            DEBUG_GET_NODE_BEGIN(nodeElement)
            ASTNode element = nodeElement->getASTNodeWithNextNode(tokenReader, cpack);
            DEBUG_GET_NODE_END(nodeElement)
            flag = element.isError();
            childNodes.push_back(std::move(element));
            if (HEDLEY_UNLIKELY(flag)) {
                return ASTNode::andNode(this, std::move(childNodes), tokenReader.collect());
            }
#if CHelperDebug == true
            if (HEDLEY_UNLIKELY(startIndex == tokenReader.index)) {
                CHELPER_WARN(L"NodeList has some error");
                return ASTNode::andNode(this, std::move(childNodes), tokenReader.collect());
            }
#endif
        }
    }

    std::optional<std::wstring> NodeList::collectDescription(const ASTNode *node, size_t index) const {
        return std::nullopt;
    }

}// namespace CHelper::Node