//
// Created by Yancey on 2023/11/10.
//

#include "NodePerCommand.h"
#include "NodeLF.h"
#include "../../resources/CPack.h"
#include "../../util/TokenUtil.h"

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
        if (j.contains("node")) {
            Profile::next(ColorStringBuilder().red("loading nodes").build());
            auto nodeJson = j.at("node");
            nodes.reserve(nodeJson.size());
            for (const auto &item: nodeJson) {
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
            if (startNodeId == "LF") {
                startNodes.push_back(NodeLF::getInstance());
                continue;
            }
            bool flag = true;
            for (auto &node: nodes) {
                if (node->id == startNodeId) {
                    startNodes.push_back(node.get());
                    flag = false;
                    break;
                }
            }
            if (flag) {
                throw Exception::UnknownNodeId(name, startNodeId);
            }
        }
        if (j.contains("ast")) {
            Profile::next(ColorStringBuilder().red("loading ast").build());
            auto ast = JsonUtil::fromJson<std::vector<std::vector<std::string>>>(j, "ast");
            for (const auto &childNodes: ast) {
                Profile::next(ColorStringBuilder().red("linking child nodes to parent node").build());
                if (childNodes.empty()) {
                    throw Exception::RequireParentNodeId(name);
                }
                auto parentNodeId = childNodes.at(0);
                Profile::next(ColorStringBuilder()
                                      .red("linking child nodes to parent node \"")
                                      .purple(parentNodeId)
                                      .red("\"")
                                      .build());
                if (childNodes.size() == 1) {
                    throw Exception::RequireChildNodeIds(name, parentNodeId);
                }
                Node::NodeBase *parentNode = nullptr;
                for (auto &node: nodes) {
                    if (node->id == parentNodeId) {
                        parentNode = node.get();
                        break;
                    }
                }
                if (parentNode == nullptr) {
                    throw Exception::UnknownNodeId(name, parentNodeId);
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
                    if (childNodeId == "LF") {
                        parentNode->nextNodes.push_back(Node::NodeLF::getInstance());
                        return;
                    }
                    Node::NodeBase *childNode = nullptr;
                    for (auto &node: nodes) {
                        if (node->id == childNodeId) {
                            childNode = node.get();
                            break;
                        }
                    }
                    if (childNode == nullptr) {
                        throw Exception::UnknownNodeId(name, childNodeId);
                    }
                    parentNode->nextNodes.push_back(childNode);
                });
            }
        }
        Profile::pop();
        for (const auto &item: nodes) {
            if (item->nextNodes.empty()) {
                throw Exception::RequireChildNodeIds(name, item->id.value_or("UNKNOWN"));
            }
        }
    }

    NodeType *NodePerCommand::getNodeType() const {
        return NodeType::PER_COMMAND.get();
    }

    void NodePerCommand::toJson(nlohmann::json &j) const {
        JsonUtil::toJson(j, "name", name);
        JsonUtil::toJsonOptional(j, "description", description);
        JsonUtil::toJson(j, "node", nodes);
        std::vector<std::string> startIds;
        startIds.reserve(startNodes.size());
        for (const auto &item: startNodes) {
            startIds.push_back(item->id.value());
        }
        JsonUtil::toJson(j, "start", startIds);
        std::vector<std::vector<std::string>> ast;
        for (const auto &item: nodes) {
            if (item->nextNodes.empty()) {
                continue;
            }
            std::vector<std::string> ast1;
            ast1.push_back(item->id.value());
            for (const auto &item2: item->nextNodes) {
                ast1.push_back(item2->id.value());
            }
            ast.push_back(ast1);
        }
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

} // CHelper::Node