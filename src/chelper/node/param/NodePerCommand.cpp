//
// Created by Yancey on 2023/11/10.
//

#include "NodePerCommand.h"
#include "../../resources/CPack.h"
#include "NodeLF.h"

namespace CHelper::Node {

    void NodePerCommand::init(const CPack &cpack) {
        for (const auto &item: nodes) {
            if (HEDLEY_LIKELY(item->id.has_value())) {
                Profile::push(ColorStringBuilder().red("init node ").purple(item->getNodeType()->nodeName).red(": \"").purple(item->id.value()).red("\"").build());
            } else {
                Profile::push(ColorStringBuilder().red("init node").build());
            }
            item->init(cpack);
            Profile::pop();
        }
        for (const auto &item: nodes) {
            if (HEDLEY_UNLIKELY(item->nextNodes.empty())) {
                throw std::runtime_error(ColorStringBuilder()
                                                 .red("dismiss child node ids, the parent node")
                                                 .normal(" -> ")
                                                 .purple(item->id.value_or("UNKNOWN"))
                                                 .red(" (in command ")
                                                 .purple(StringUtil::join(",", name))
                                                 .red(")")
                                                 .build());
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
                    throw std::runtime_error(ColorStringBuilder()
                                                     .purple("isMustAfterWhiteSpace")
                                                     .red(" should be ")
                                                     .purple(item2->isMustAfterWhiteSpace ? "false" : "true")
                                                     .red(" in node \"")
                                                     .purple(item2->id.value_or("unknown"))
                                                     .red("\"")
                                                     .build());
                }
            }
        }
#endif
    }

    NodeType *NodePerCommand::getNodeType() const {
        return NodeType::PER_COMMAND.get();
    }

    ASTNode NodePerCommand::getASTNode(TokenReader &tokenReader, const CPack *cpack) const {
        std::vector<ASTNode> childASTNodes;
        childASTNodes.reserve(startNodes.size());
        for (const auto &item: startNodes) {
            tokenReader.push();
            DEBUG_GET_NODE_BEGIN(item)
            childASTNodes.push_back(item->getASTNodeWithNextNode(
                    tokenReader, cpack, item->getNodeType() != NodeType::REPEAT.get()));
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

#if CHelperSupportJson == true
    void from_json(const nlohmann::json &j, std::unique_ptr<NodePerCommand> &t) {
        t = std::make_unique<NodePerCommand>();
        //name
        Profile::push(ColorStringBuilder().red("loading node name").build());
        JsonUtil::decode(j, "name", t->name);
        //description
        Profile::next(ColorStringBuilder().red("loading node description").build());
        JsonUtil::decode(j, "description", t->description);
        //node
        if (HEDLEY_LIKELY(j.contains("node"))) {
            JsonUtil::decode(j, "node", t->nodes);
        }
        //start
        Profile::next(ColorStringBuilder().red("loading start nodes").build());
        auto startNodeIds = JsonUtil::read<std::vector<std::string>>(j, "start");
        t->startNodes.reserve(startNodeIds.size());
        for (const auto &startNodeId: startNodeIds) {
            Profile::next(ColorStringBuilder()
                                  .red("linking startNode \"")
                                  .purple(startNodeId)
                                  .red("\" to nodes")
                                  .build());
            if (HEDLEY_UNLIKELY(startNodeId == "LF")) {
                t->startNodes.push_back(NodeLF::getInstance());
                continue;
            }
            bool flag = true;
            for (auto &node1: t->nodes) {
                if (HEDLEY_UNLIKELY(node1->id == startNodeId)) {
                    t->startNodes.push_back(node1.get());
                    flag = false;
                    break;
                }
            }
            if (HEDLEY_UNLIKELY(flag)) {
                throw std::runtime_error(ColorStringBuilder()
                                                 .red("unknown node id")
                                                 .normal(" -> ")
                                                 .purple(startNodeId)
                                                 .red(" (in command \"")
                                                 .purple(StringUtil::join(",", t->name))
                                                 .red("\")")
                                                 .build());
            }
        }
        //ast
        auto jsonAst = j.find("ast");
        if (HEDLEY_LIKELY(jsonAst != j.end())) {
            Profile::next(ColorStringBuilder().red("loading ast").build());
            for (const auto &childNodes: jsonAst->get<std::vector<std::vector<std::string>>>()) {
                Profile::next(ColorStringBuilder().red("linking child nodes to parent node").build());
                if (HEDLEY_UNLIKELY(childNodes.empty())) {
                    throw std::runtime_error(ColorStringBuilder()
                                                     .red("dismiss parent node id")
                                                     .red(" (in command ")
                                                     .purple(StringUtil::join(",", t->name))
                                                     .red(")")
                                                     .build());
                }
                auto parentNodeId = childNodes.at(0);
                Profile::next(ColorStringBuilder()
                                      .red("linking child nodes to parent node \"")
                                      .purple(parentNodeId)
                                      .red("\"")
                                      .build());
                if (HEDLEY_UNLIKELY(childNodes.size() == 1)) {
                    throw std::runtime_error(ColorStringBuilder()
                                                     .red("dismiss child node ids, the parent node")
                                                     .normal(" -> ")
                                                     .purple(parentNodeId)
                                                     .red(" (in command ")
                                                     .purple(StringUtil::join(",", t->name))
                                                     .red(")")
                                                     .build());
                }
                Node::NodeBase *parentNode = nullptr;
                for (auto &node1: t->nodes) {
                    if (HEDLEY_UNLIKELY(node1->id == parentNodeId)) {
                        parentNode = node1.get();
                        break;
                    }
                }
                if (HEDLEY_UNLIKELY(parentNode == nullptr)) {
                    throw std::runtime_error(ColorStringBuilder()
                                                     .red("unknown node id")
                                                     .normal(" -> ")
                                                     .purple(parentNodeId)
                                                     .red(" (in command \"")
                                                     .purple(StringUtil::join(",", t->name))
                                                     .red("\")")
                                                     .build());
                }
                if (HEDLEY_UNLIKELY(!parentNode->nextNodes.empty())) {
                    throw std::runtime_error(ColorStringBuilder()
                                                     .red("repeating parent node \"")
                                                     .purple(parentNode->id.value())
                                                     .red("\"")
                                                     .build());
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
                    for (auto &node: t->nodes) {
                        if (HEDLEY_UNLIKELY(node->id == childNodeId)) {
                            childNode = node.get();
                            break;
                        }
                    }
                    if (HEDLEY_UNLIKELY(childNode == nullptr)) {
                        throw std::runtime_error(ColorStringBuilder()
                                                         .red("unknown node id")
                                                         .normal(" -> ")
                                                         .purple(childNodeId)
                                                         .red(" (in command \"")
                                                         .purple(StringUtil::join(",", t->name))
                                                         .red("\")")
                                                         .build());
                    }
                    parentNode->nextNodes.push_back(childNode);
                });
            }
        }
        Profile::pop();
    }

    void to_json(nlohmann::json &j, const std::unique_ptr<NodePerCommand> &t) {
        //name
        JsonUtil::encode(j, "name", t->name);
        //description
        JsonUtil::encode(j, "description", t->description);
        //node
        JsonUtil::encode(j, "node", t->nodes);
        //start
        std::vector<std::string> startIds;
        startIds.reserve(t->startNodes.size());
        for (const auto &item: t->startNodes) {
            startIds.push_back(item->id.value());
        }
        JsonUtil::encode(j, "start", startIds);
        //ast
        std::vector<std::vector<std::string>> ast;
        for (const auto &item: t->nodes) {
            std::vector<std::string> ast1;
            ast1.push_back(item->id.value());
            for (const auto &item2: item->nextNodes) {
                ast1.push_back(item2->id.value());
            }
            ast.push_back(std::move(ast1));
        }
        JsonUtil::encode(j, "ast", ast);
    }

#endif
    void from_binary(BinaryReader &binaryReader, std::unique_ptr<NodePerCommand> &t) {
        t = std::make_unique<NodePerCommand>();
        //name
        binaryReader.decode(t->name);
        if (HEDLEY_UNLIKELY(t->name.empty())) {
            throw std::runtime_error("command size cannot be zero");
        }
        binaryReader.decode(t->description);
        //node
        binaryReader.decode(t->nodes);
        //start
        size_t startNodeIdSize = binaryReader.readSize();
        t->startNodes.reserve(startNodeIdSize);
        for (int i = 0; i < startNodeIdSize; ++i) {
            auto startNodeId = binaryReader.read<std::string>();
            if (HEDLEY_UNLIKELY(startNodeId == "LF")) {
                t->startNodes.push_back(NodeLF::getInstance());
                continue;
            }
            bool flag = true;
            for (auto &node: t->nodes) {
                if (HEDLEY_UNLIKELY(node->id == startNodeId)) {
                    t->startNodes.push_back(node.get());
                    flag = false;
                    break;
                }
            }
            if (HEDLEY_UNLIKELY(flag)) {
                throw std::runtime_error(ColorStringBuilder()
                                                 .red("unknown node id")
                                                 .normal(" -> ")
                                                 .purple(startNodeId)
                                                 .red(" (in command \"")
                                                 .purple(StringUtil::join(",", t->name))
                                                 .red("\")")
                                                 .build());
            }
        }
        //ast
        for (const auto &node: t->nodes) {
            Node::NodeBase *parentNode = node.get();
            size_t childNodeSize = binaryReader.readSize();
            parentNode->nextNodes.reserve(childNodeSize);
            for (int j = 0; j < childNodeSize; ++j) {
                auto childNodeId = binaryReader.read<std::string>();
                if (HEDLEY_UNLIKELY(childNodeId == "LF")) {
                    parentNode->nextNodes.push_back(Node::NodeLF::getInstance());
                    continue;
                }
                Node::NodeBase *childNode = nullptr;
                for (auto &node1: t->nodes) {
                    if (HEDLEY_UNLIKELY(node1->id == childNodeId)) {
                        childNode = node1.get();
                        break;
                    }
                }
                if (HEDLEY_UNLIKELY(childNode == nullptr)) {
                    throw std::runtime_error(ColorStringBuilder()
                                                     .red("unknown node id")
                                                     .normal(" -> ")
                                                     .purple(childNodeId)
                                                     .red(" (in command \"")
                                                     .purple(StringUtil::join(",", t->name))
                                                     .red("\")")
                                                     .build());
                }
                parentNode->nextNodes.push_back(childNode);
            }
        }
    }

#if CHelperWeb != true
    void to_binary(BinaryWriter &binaryWriter, const std::unique_ptr<NodePerCommand> &t) {
        //name
        binaryWriter.encode(t->name);
        //description
        binaryWriter.encode(t->description);
        //node
        binaryWriter.encode(t->nodes);
        //start
        binaryWriter.encodeSize(t->startNodes.size());
        for (const auto &item: t->startNodes) {
            binaryWriter.encode(item->id.value());
        }
        //ast
        for (const auto &item: t->nodes) {
            binaryWriter.encodeSize(item->nextNodes.size());
            for (const auto &item2: item->nextNodes) {
                binaryWriter.encode(item2->id.value());
            }
        }
    }
#endif

}// namespace CHelper::Node