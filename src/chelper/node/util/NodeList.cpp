//
// Created by Yancey on 2023/12/23.
//

#include "NodeList.h"
#include "NodeOr.h"

namespace CHelper::Node {

    NodeList::NodeList(const std::optional<std::string> &id,
                       const std::optional<std::string> &description,
                       const std::shared_ptr<NodeBase> &nodeLeft,
                       const std::shared_ptr<NodeBase> &nodeElement,
                       const std::shared_ptr<NodeBase> &nodeSeparator,
                       const std::shared_ptr<NodeBase> &nodeRight)
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
        auto left = nodeLeft->getASTNode(tokenReader);
        std::vector<ASTNode> childNodes = {left};
        if (left.isError()) {
            return ASTNode::andNode(this, childNodes, tokenReader.collect());
        }
        {
            //检测[]中间有没有内容
            tokenReader.push();
            DEBUG_GET_NODE_BEGIN(nodeRight)
            auto rightBracket1 = nodeRight->getASTNode(tokenReader);
            DEBUG_GET_NODE_END(nodeRight)
            tokenReader.restore();
            DEBUG_GET_NODE_BEGIN(nodeElementOrRight)
            auto elementOrRight = nodeElementOrRight->getASTNode(tokenReader);
            DEBUG_GET_NODE_END(nodeElementOrRight)
            childNodes.push_back(elementOrRight);
            if (!rightBracket1.isError() || elementOrRight.isError()) {
                return ASTNode::andNode(this, childNodes, tokenReader.collect());
            }
        }
        while (true) {
            //检测是分隔符还是右括号
            tokenReader.push();
            DEBUG_GET_NODE_BEGIN(nodeRight)
            auto rightBracket = nodeRight->getASTNode(tokenReader);
            DEBUG_GET_NODE_END(nodeRight)
            tokenReader.restore();
            DEBUG_GET_NODE_BEGIN(nodeSeparator)
            auto separatorOrRight = nodeSeparatorOrRight->getASTNode(tokenReader);
            DEBUG_GET_NODE_END(nodeSeparator)
            childNodes.push_back(separatorOrRight);
            if (!rightBracket.isError() || separatorOrRight.isError()) {
                return ASTNode::andNode(this, childNodes, tokenReader.collect());
            }
            //检测是不是元素
            DEBUG_GET_NODE_BEGIN(nodeElement)
            auto element = nodeElement->getASTNode(tokenReader);
            DEBUG_GET_NODE_END(nodeElement)
            childNodes.push_back(element);
            if (element.isError()) {
                return ASTNode::andNode(this, childNodes, tokenReader.collect());
            }
        }
    }

    std::optional<std::string> NodeList::collectDescription(const ASTNode *node, size_t index) const {
        return std::nullopt;
    }

} // CHelper::Node