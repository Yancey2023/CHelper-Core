//
// Created by Yancey on 2024/3/1.
//

#include <chelper/node/param/NodeRepeat.h>
#include <chelper/node/util/NodeAnd.h>

namespace CHelper::Node {

    void NodeRepeat::init(const CPack &cpack) {
        const auto &it = cpack.repeatNodes.find(key);
        if (HEDLEY_LIKELY(it != cpack.repeatNodes.end())) {
            repeatData = it->second.first;
            nodeElement = it->second.second;
            return;
        }
        Profile::push("link repeat data {} to content", key);
        Profile::push("fail to find repeat data by id {}", key);
        throw std::runtime_error("fail to find repeat data");
    }

    NodeTypeId::NodeTypeId NodeRepeat::getNodeType() const {
        return NodeTypeId::REPEAT;
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

    std::optional<std::u16string> NodeRepeat::collectDescription(const ASTNode *node, size_t index) const {
        if (HEDLEY_UNLIKELY(node->tokens.isEmpty())) {
            return description;
        } else {
            return std::nullopt;
        }
    }

    void NodeRepeat::collectStructure(const ASTNode *astNode,
                                      StructureBuilder &structure,
                                      bool isMustHave) const {
        if (astNode != nullptr) {
            for (const auto &item: astNode->childNodes) {
                //如果内容为空，就跳过
                if (HEDLEY_UNLIKELY(item.tokens.isEmpty() || item.tokens.isAllWhitespace())) {
                    continue;
                }
                //获取结构
                const ASTNode &astNode1 = HEDLEY_LIKELY(item.whichBest == 0)
                                                  ? item.getBestNode().getBestNode()
                                                  : item.getBestNode();
                auto node1 = reinterpret_cast<const NodeAnd *>(astNode1.node);
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
        structure.appendWhiteSpace().append(u"...");
        for (const auto &item: reinterpret_cast<const NodeAnd *>(reinterpret_cast<const NodeOr *>(nodeElement)->childNodes[1])->childNodes) {
            item->collectStructure(nullptr, structure, true);
        }
        structure.isDirty = true;
    }

}// namespace CHelper::Node