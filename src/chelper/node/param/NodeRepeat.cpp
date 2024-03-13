//
// Created by Yancey on 2024/3/1.
//

#include "NodeRepeat.h"

#include "../../resources/CPack.h"

namespace CHelper::Node {

    NodeRepeat::NodeRepeat(const std::optional<std::string> &id,
                           const std::optional<std::string> &description,
                           std::string key,
                           const std::pair<NodeBase *, NodeBase *> &node)
            : NodeBase(id, description, false),
              key(std::move(key)),
              node(node) {}

    std::pair<Node::NodeBase *, Node::NodeBase *> getNodeFromCPack(const std::string &key,
                                                                   [[maybe_unused]] const CPack &cpack) {
        for (const auto &item: cpack.repeatNodes) {
            if (item.first->id == key) {
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
              node(getNodeFromCPack(key, cpack)) {}

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
            tokenReader.push();
            ASTNode astNode = node.first->getASTNodeWithNextNode(tokenReader, cpack);
            size_t astNodeIndex = tokenReader.index;
            tokenReader.restore();
            tokenReader.push();
            ASTNode breakAstNode = node.second->getASTNodeWithNextNode(tokenReader, cpack);
            size_t breakAstNodeIndex = tokenReader.index;
            tokenReader.restore();
            tokenReader.push();
            tokenReader.index = breakAstNode.isError() ? astNodeIndex : breakAstNodeIndex;
            ASTNode orNode = ASTNode::orNode(this, {astNode, breakAstNode}, tokenReader.collect());
            childNodes.push_back(orNode);
            if (!breakAstNode.isError()) {
                return ASTNode::andNode(this, std::move(childNodes), tokenReader.collect());
            } else if (orNode.isError() || !tokenReader.ready() || orNode.tokens.isEmpty()) {
                VectorView <Token> tokens = tokenReader.collect();
                return ASTNode::andNode(this, std::move(childNodes), tokens,
                                        astNode.isError() ? nullptr : ErrorReason::incomplete(
                                                tokens, "命令重复部分缺少结束语句"),
                                        orNode.whichBest == 0 ? "repeat no complete1" : "repeat no complete2");
            }
        }
    }

    void NodeRepeat::collectStructure(const ASTNode *astNode,
                                      StructureBuilder &structure,
                                      bool isMustHave) const {
        for (const auto &item: astNode->childNodes[0].childNodes) {
            structure.appendWhiteSpace().append(item.getStructure());
        }
        if (astNode->id == "repeat no complete1") {
            structure.appendWhiteSpace().append("...");
        } else if (astNode->id == "repeat no complete2") {
            node.second->collectStructure(nullptr, structure, isMustHave);
            structure.appendWhiteSpace().append("...");
        }
    }

} // CHelper::Node