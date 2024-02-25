//
// Created by Yancey on 2023/12/23.
//

#include "NodeList.h"

namespace CHelper::Node {

    NodeList::NodeList(const std::optional<std::string> &id,
                       const std::optional<std::string> &description,
                       std::shared_ptr<NodeBase> &nodeLeft,
                       std::shared_ptr<NodeBase> &nodeElement,
                       std::shared_ptr<NodeBase> &nodeSeparator,
                       std::shared_ptr<NodeBase> &nodeRight)
            : NodeBase(id, description),
              nodeLeft(nodeLeft),
              nodeElement(nodeElement),
              nodeSeparator(nodeSeparator),
              nodeRight(nodeRight) {}

    ASTNode NodeList::getASTNode(TokenReader &tokenReader) const {
        //标记整个[...]，在最后进行收集
        tokenReader.push();
        std::vector<ASTNode> childNodes = {nodeLeft->getASTNode(tokenReader)};
        //检测[]中间有没有内容
        tokenReader.push();
        auto rightBracket = nodeRight->getASTNode(tokenReader);
        if (!rightBracket.isError()) {
            // []中间没有内容
            tokenReader.pop();
            childNodes.push_back(rightBracket);
        } else {
            tokenReader.restore();
            // [后面的第一个元素
            childNodes.push_back(nodeElement->getASTNode(tokenReader));
            while (true) {
                //检测是不是分隔符
                tokenReader.push();
                auto separator = nodeSeparator->getASTNode(tokenReader);
                if (separator.isError()) {
                    //不是分隔符，结束
                    tokenReader.restore();
                    childNodes.push_back(nodeRight->getASTNode(tokenReader));
                    break;
                }
                tokenReader.pop();
                //下一个元素
                childNodes.push_back(nodeElement->getASTNode(tokenReader));
            }
            childNodes.push_back(nodeRight->getASTNode(tokenReader));
        }
        //返回结果
        return ASTNode::andNode(this, childNodes, tokenReader.collect());
    }

    std::optional<std::string> NodeList::collectDescription(const ASTNode *node, size_t index) const {
        return std::nullopt;
    }

} // CHelper::Node