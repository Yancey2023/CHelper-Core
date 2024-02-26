//
// Created by Yancey on 2023/12/23.
//

#include "NodeList.h"
#include "NodeOr.h"

namespace CHelper::Node {

    NodeList::NodeList(const std::optional<std::string> &id,
                       const std::optional<std::string> &description,
                       std::shared_ptr<NodeBase> &nodeLeft,
                       std::shared_ptr<NodeBase> &nodeElement,
                       std::shared_ptr<NodeBase> &nodeSeparator,
                       std::shared_ptr<NodeBase> &nodeRight)
            : NodeBase(id, description, false),
              nodeLeft(nodeLeft),
              nodeElement(nodeElement),
              nodeSeparator(nodeSeparator),
              nodeRight(nodeRight),
              nodeElementOrRight(std::make_shared<NodeOr>(
                      "ELEMENT_OR_RIGHT", "element or right",
                      std::make_shared<std::vector<std::shared_ptr<NodeBase>>>(
                              std::vector<std::shared_ptr<NodeBase>>{
                                      nodeElement, nodeRight
                              }), false)),
              nodeSeparatorOrRight(std::make_shared<NodeOr>(
                      "SEPARATOR_OR_RIGHT", "separator or right",
                      std::make_shared<std::vector<std::shared_ptr<NodeBase>>>(
                              std::vector<std::shared_ptr<NodeBase>>{
                                      nodeSeparator, nodeRight
                              }), false)) {}

    ASTNode NodeList::getASTNode(TokenReader &tokenReader) const {
        //标记整个[...]，在最后进行收集
        tokenReader.push();
        std::vector<ASTNode> childNodes = {nodeLeft->getASTNode(tokenReader)};
        {
            //检测[]中间有没有内容
            tokenReader.push();
            DEBUG_GET_NODE_BEGIN(nodeRight)
            auto rightBracket = nodeRight->getASTNode(tokenReader);
            DEBUG_GET_NODE_END(nodeRight)
            if (!rightBracket.isError()) {
                tokenReader.pop();
                childNodes.push_back(rightBracket);
                return ASTNode::andNode(this, childNodes, tokenReader.collect());
            }
            tokenReader.restore();
            DEBUG_GET_NODE_BEGIN(nodeElementOrRight)
            auto elementOrRight = nodeElementOrRight->getASTNode(tokenReader);
            DEBUG_GET_NODE_END(nodeElementOrRight)
            childNodes.push_back(elementOrRight);
            if (elementOrRight.isError()) {
                return ASTNode::andNode(this, childNodes, tokenReader.collect());
            }
        }
        while (true) {
            //检测是分隔符还是右括号
            tokenReader.push();
            DEBUG_GET_NODE_BEGIN(nodeRight)
            auto rightBracket = nodeRight->getASTNode(tokenReader);
            DEBUG_GET_NODE_END(nodeRight)
            auto rightBracketIndex = tokenReader.index;
            tokenReader.restore();
            tokenReader.push();
            DEBUG_GET_NODE_BEGIN(nodeSeparator)
            auto separator = nodeSeparator->getASTNode(tokenReader);
            DEBUG_GET_NODE_END(nodeSeparator)
            auto separatorIndex = tokenReader.index;
            tokenReader.restore();
            if (!rightBracket.isError() || separator.tokens.isEmpty()) {
                //不是分隔符，结束
                tokenReader.index = rightBracketIndex;
                childNodes.push_back(ASTNode::orNode(this, {rightBracket, separator}));
                return ASTNode::andNode(this, childNodes, tokenReader.collect());
            }
            tokenReader.index = separatorIndex;
            if (separator.isError()) {
                childNodes.push_back(separator);
                return ASTNode::andNode(this, childNodes, tokenReader.collect());
            }
            //检测是不是元素
            DEBUG_GET_NODE_BEGIN(nodeElement)
            auto element = nodeElement->getASTNode(tokenReader);
            DEBUG_GET_NODE_END(nodeElement)
            childNodes.push_back(element);
            if (element.isError()) {
                //不是元素，错误
                return ASTNode::andNode(this, childNodes, tokenReader.collect());
            }
        }
    }

    std::optional<std::string> NodeList::collectDescription(const ASTNode *node, size_t index) const {
        return std::nullopt;
    }

} // CHelper::Node