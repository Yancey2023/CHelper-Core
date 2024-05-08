//
// Created by Yancey on 2023/11/10.
//

#include "NodePerCommand.h"
#include "NodeLF.h"
#include "../../resources/CPack.h"

namespace CHelper::Node {

    NodePerCommand::NodePerCommand(const std::optional<std::string> &id,
                                   std::vector<std::string> name,
                                   const std::optional<std::string> &description,
                                   std::vector<std::unique_ptr<Node::NodeBase>> nodes,
                                   std::vector<Node::NodeBase *> &startNodes)
            : NodeBase(id, description, false),
              name(std::move(name)),
              nodes(std::move(nodes)),
              startNodes(startNodes) {}

    NodePerCommand::NodePerCommand(const nlohmann::json &j,
                                   const CPack &cpack)
            : NodeBase(j, false) {
        Profile::push(ColorStringBuilder().red("loading node name").build());
        name = JsonUtil::fromJson<std::vector<std::string>>(j, "name");
        auto nodeJson = j.find("node");
        if (HEDLEY_LIKELY(nodeJson != j.end())) {
            Profile::next(ColorStringBuilder().red("loading nodes").build());
            nodes.reserve(nodeJson->size());
            for (const auto &item: nodeJson.value()) {
                nodes.push_back(getNodeFromJson(item, cpack));
            }
        }
        Profile::next(ColorStringBuilder().red("loading start nodes").build());
        auto startNodeIds = JsonUtil::fromJson<std::vector<std::string>>(j, "start");
        startNodes.reserve(startNodeIds.size());
        for (const auto &startNodeId: startNodeIds) {
            Profile::next(ColorStringBuilder()
                                  .red("linking startNode \"")
                                  .purple(startNodeId)
                                  .red("\" to nodes")
                                  .build());
            if (HEDLEY_UNLIKELY(startNodeId == "LF")) {
                startNodes.push_back(NodeLF::getInstance());
                continue;
            }
            bool flag = true;
            for (auto &node: nodes) {
                if (HEDLEY_UNLIKELY(node->id == startNodeId)) {
                    startNodes.push_back(node.get());
                    flag = false;
                    break;
                }
            }
            if (HEDLEY_UNLIKELY(flag)) {
                throw Exception::UnknownNodeId(name, startNodeId);
            }
        }
        auto jsonAst = j.find("ast");
        if (HEDLEY_LIKELY(jsonAst != j.end())) {
            Profile::next(ColorStringBuilder().red("loading ast").build());
            for (const auto &childNodes: jsonAst->get<std::vector<std::vector<std::string>>>()) {
                Profile::next(ColorStringBuilder().red("linking child nodes to parent node").build());
                if (HEDLEY_UNLIKELY(childNodes.empty())) {
                    throw Exception::RequireParentNodeId(name);
                }
                auto parentNodeId = childNodes.at(0);
                Profile::next(ColorStringBuilder()
                                      .red("linking child nodes to parent node \"")
                                      .purple(parentNodeId)
                                      .red("\"")
                                      .build());
                if (HEDLEY_UNLIKELY(childNodes.size() == 1)) {
                    throw Exception::RequireChildNodeIds(name, parentNodeId);
                }
                Node::NodeBase *parentNode = nullptr;
                for (auto &node: nodes) {
                    if (HEDLEY_UNLIKELY(node->id == parentNodeId)) {
                        parentNode = node.get();
                        break;
                    }
                }
                if (HEDLEY_UNLIKELY(parentNode == nullptr)) {
                    throw Exception::UnknownNodeId(name, parentNodeId);
                }
                if (!HEDLEY_UNLIKELY(parentNode->nextNodes.empty())) {
                    Profile::next(ColorStringBuilder()
                                          .red("repeating parent node \"")
                                          .purple(parentNode->id.value())
                                          .red("\"")
                                          .build());
                    throw Exception::NodeLoadFailed();
                }
                parentNode->nextNodes.reserve(childNodes.size() - 1);
                for_each(childNodes.begin() + 1, childNodes.end(), [&](const auto &childNodeId) {
                    Profile::next(ColorStringBuilder()
                                          .red("linking child nodes \"")
                                          .purple(childNodeId)
                                          .red("\" to parent node \"")
                                          .purple(parentNodeId)
                                          .red("\"")
                                          .build());
                    if (HEDLEY_UNLIKELY(childNodeId == "LF")) {
                        parentNode->nextNodes.push_back(Node::NodeLF::getInstance());
                        return;
                    }
                    Node::NodeBase *childNode = nullptr;
                    for (auto &node: nodes) {
                        if (HEDLEY_UNLIKELY(node->id == childNodeId)) {
                            childNode = node.get();
                            break;
                        }
                    }
                    if (HEDLEY_UNLIKELY(childNode == nullptr)) {
                        throw Exception::UnknownNodeId(name, childNodeId);
                    }
                    parentNode->nextNodes.push_back(childNode);
                });
            }
        }
        Profile::pop();
        for (const auto &item: nodes) {
            if (HEDLEY_UNLIKELY(item->nextNodes.empty())) {
                throw Exception::RequireChildNodeIds(name, item->id.value_or("UNKNOWN"));
            }
        }
#if CHelperDebug == true
        for (const auto &item: nodes) {
            bool flag1 = item->getNodeType() == NodeType::POSITION.get() ||
                         item->getNodeType() == NodeType::RELATIVE_FLOAT.get();
            for (const auto &item2: item->nextNodes) {
                if (HEDLEY_UNLIKELY(item2 == NodeLF::getInstance())) {
                    continue;
                }
                bool flag2 = item2->getNodeType() == NodeType::POSITION.get() ||
                             item2->getNodeType() == NodeType::RELATIVE_FLOAT.get();
                if (HEDLEY_UNLIKELY(flag1 && flag2 == item2->isMustAfterWhiteSpace)) {
                    Profile::push(ColorStringBuilder()
                                          .purple("isMustAfterWhiteSpace")
                                          .red(" should be ")
                                          .purple(item2->isMustAfterWhiteSpace ? "false" : "true")
                                          .red(" in node \"")
                                          .purple(item2->id.value_or("unknown"))
                                          .red("\"")
                                          .build());
                    throw Exception::NodeLoadFailed();
                }
            }
        }
#endif
    }

    NodeType *NodePerCommand::getNodeType() const {
        return NodeType::PER_COMMAND.get();
    }

    void NodePerCommand::toJson(nlohmann::json &j) const {
        JsonUtil::toJson(j, "name", name);
        JsonUtil::toJsonOptionalLikely(j, "description", description);
        JsonUtil::toJson(j, "node", nodes);
        std::vector<std::string> startIds;
        startIds.reserve(startNodes.size());
        for (const auto &item: startNodes) {
            startIds.push_back(item->id.value());
        }
        JsonUtil::toJson(j, "start", startIds);
        std::vector<std::vector<std::string>> ast;
        for (const auto &item: nodes) {
            std::vector<std::string> ast1;
            ast1.push_back(item->id.value());
            for (const auto &item2: item->nextNodes) {
                ast1.push_back(item2->id.value());
            }
            ast.push_back(std::move(ast1));
        }
        JsonUtil::toJson(j, "ast", ast);
    }

    ASTNode NodePerCommand::getASTNode(TokenReader &tokenReader, const CPack *cpack) const {
        std::vector<ASTNode> childASTNodes;
        childASTNodes.reserve(startNodes.size());
        for (const auto &item: startNodes) {
            tokenReader.push();
            DEBUG_GET_NODE_BEGIN(item)
            childASTNodes.push_back(item->getASTNodeWithNextNode(tokenReader, cpack));
            DEBUG_GET_NODE_END(item)
            tokenReader.restore();
        }
        tokenReader.push();
        tokenReader.skipToLF();
        return ASTNode::orNode(this, std::move(childASTNodes), tokenReader.collect());
    }

    std::optional<std::string> NodePerCommand::collectDescription(const ASTNode *node, size_t index) const {
        return std::nullopt;
    }

} // CHelper::Node