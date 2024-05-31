//
// Created by Yancey on 2024/3/1.
//

#include "NodeRepeat.h"

#include "../util/NodeAnd.h"

namespace CHelper::Node {

    void NodeRepeat::init(const CPack &cpack) {
        const auto &it = cpack.repeatNodes.find(key);
        if (HEDLEY_LIKELY(it != cpack.repeatNodes.end())) {
            repeatData = it->second.first;
            nodeElement = it->second.second;
            return;
        }
        Profile::push(ColorStringBuilder()
                              .red("link repeat data ")
                              .purple(key)
                              .red(" to content")
                              .build());
        Profile::push(ColorStringBuilder()
                              .red("fail to find repeat data by id ")
                              .purple(key)
                              .build());
        throw Exception::NodeLoadFailed();
    }

    NodeType *NodeRepeat::getNodeType() const {
        return NodeType::REPEAT.get();
    }

    ASTNode NodeRepeat::getASTNode(TokenReader &tokenReader, const CPack *cpack) const {
        tokenReader.push();
        std::vector<ASTNode> childNodes;
        while (true) {
            ASTNode orNode = nodeElement->getASTNode(tokenReader, cpack);
            bool isAstNodeError = orNode.childNodes[0].isError();
            bool isBreakAstNodeError = orNode.childNodes[1].isError();
            if (HEDLEY_UNLIKELY(!isBreakAstNodeError || isAstNodeError ||
                                (!tokenReader.ready() && repeatData->isEnd[orNode.childNodes[0].whichBest]))) {
                childNodes.push_back(std::move(orNode));
                return ASTNode::andNode(this, std::move(childNodes), tokenReader.collect(), nullptr);
            }
            childNodes.push_back(std::move(orNode));
        }
    }

    void NodeRepeat::collectStructure(const ASTNode *astNode,
                                      StructureBuilder &structure,
                                      bool isMustHave) const {
        if (astNode != nullptr) {
            for (const auto &item: astNode->childNodes) {
                //如果内容为空，就跳过
                if (HEDLEY_UNLIKELY(item.tokens.isEmpty() ||
                                    std::all_of(item.tokens.beginIterator(), item.tokens.endIterator(),
                                                [](const auto &item) {
                                                    return item.type == TokenType::WHITE_SPACE;
                                                }))) {
                    continue;
                }
                //获取结构
                const ASTNode &astNode1 = HEDLEY_LIKELY(item.whichBest == 0)
                                                  ? item.getBestNode().getBestNode()
                                                  : item.getBestNode();
                const NodeAnd *node1 = (NodeAnd *) astNode1.node;
                size_t astNodeSize = astNode1.childNodes.size();
                size_t nodeSize = node1->childNodes.size();
                structure.isDirty = false;
                for (size_t i = 0; i < nodeSize; ++i) {
                    if (HEDLEY_LIKELY(i < astNodeSize)) {
                        astNode1.childNodes[i].collectStructure(structure, true);
                    } else {
                        node1->childNodes[i]->collectStructureWithNextNodes(structure, true);
                    }
                }
                if (HEDLEY_UNLIKELY(item.whichBest == 1)) {
                    //如果是结束语句，就不继续执行了
                    structure.isDirty = true;
                    return;
                }
            }
        }
        //如果没有遇到结束语句，添加...和结束语句的结构
        structure.appendWhiteSpace().append("...");
        for (const auto &item: ((NodeAnd *) ((NodeOr *) nodeElement)->childNodes[1])->childNodes) {
            item->collectStructure(nullptr, structure, true);
        }
        structure.isDirty = true;
    }

    CODEC_NODE(NodeRepeat, key)

}// namespace CHelper::Node