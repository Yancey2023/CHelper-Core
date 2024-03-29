//
// Created by Yancey on 2024/3/1.
//

#include "NodeRepeat.h"

#include "../../resources/CPack.h"
#include "../util/NodeAnd.h"

namespace CHelper::Node {

    static NodeOr getNodeElement(const std::pair<NodeBase *, NodeBase *> &node) {
        return NodeOr("NODE_REPEAT", "命令重复部分", {node.first, node.second}, false);
    }

    NodeRepeat::NodeRepeat(const std::optional<std::string> &id,
                           const std::optional<std::string> &description,
                           std::string key,
                           const std::pair<NodeBase *, NodeBase *> &node)
            : NodeBase(id, description, true),
              key(std::move(key)),
              nodeElement(getNodeElement(node)) {}

    std::pair<Node::NodeBase *, Node::NodeBase *> getNodeFromCPack(const std::string &key,
                                                                   [[maybe_unused]] const CPack &cpack) {
        for (const auto &item: cpack.repeatNodes) {
            if (HEDLEY_UNLIKELY(item.first->id == key)) {
                return item;
            }
        }
        Profile::push(ColorStringBuilder()
                              .red("link repeat data ")
                              .purple(key)
                              .red("to content")
                              .build());
        Profile::push(ColorStringBuilder()
                              .red("fail to find repeat data by id ")
                              .purple(key)
                              .build());
        throw Exception::NodeLoadFailed();
    }

    NodeRepeat::NodeRepeat(const nlohmann::json &j,
                           const CPack &cpack)
            : NodeBase(j, false),
              key(JsonUtil::fromJson<std::string>(j, "key")),
              nodeElement(getNodeElement(getNodeFromCPack(key, cpack))) {}

    NodeType *NodeRepeat::getNodeType() const {
        return NodeType::REPEAT.get();
    }

    void NodeRepeat::toJson(nlohmann::json &j) const {
        NodeBase::toJson(j);
        JsonUtil::toJson(j, "key", key);
    }

    ASTNode NodeRepeat::getASTNode(TokenReader &tokenReader, const CPack *cpack) const {
        tokenReader.push();
        std::vector<ASTNode> childNodes;
        while (true) {
            ASTNode orNode = nodeElement.getASTNode(tokenReader, cpack);
            bool isAstNodeError = orNode.childNodes[0].isError();
            bool isBreakAstNodeError = orNode.childNodes[1].isError();
            childNodes.push_back(std::move(orNode));
            if (HEDLEY_UNLIKELY(!isBreakAstNodeError || isAstNodeError || !tokenReader.ready())) {
                return ASTNode::andNode(this, std::move(childNodes), tokenReader.collect(), nullptr);
            }
        }
    }

    void NodeRepeat::collectStructure(const ASTNode *astNode,
                                      StructureBuilder &structure,
                                      bool isMustHave) const {
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
                                      ? item.getBestNode().getBestNode() : item.getBestNode();
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
        //如果没有遇到结束语句，添加...和结束语句的结构
        structure.appendWhiteSpace().append("...");
        for (const auto &item: ((NodeAnd *) nodeElement.childNodes[1])->childNodes) {
            item->collectStructure(nullptr, structure, true);
        }
        structure.isDirty = true;
    }

} // CHelper::Node