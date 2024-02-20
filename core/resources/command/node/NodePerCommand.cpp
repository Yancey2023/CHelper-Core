//
// Created by Yancey666 on 2023/11/10.
//

#include "NodePerCommand.h"
#include "NodeLF.h"
#include "../../../util/StringUtil.h"

namespace CHelper::Node {

    NODE_TYPE("PER_COMMAND", NodePerCommand)

    NodePerCommand::NodePerCommand(const std::optional<std::string> &id,
                                   std::vector<std::string> name,
                                   const std::optional<std::string> &description,
                                   std::vector<std::shared_ptr<Node::NodeBase>> nodes,
                                   std::vector<std::shared_ptr<Node::NodeBase>> startNodes)
            : NodeBase(id, description),
              name(std::move(name)),
              nodes(std::move(nodes)),
              startNodes(std::move(startNodes)) {}

    NodePerCommand::NodePerCommand(const nlohmann::json &j,
                                   const CPack &cpack)
            : NodeBase(j, cpack) {
        std::string state = Color::RED + "unknown state" +
                            Color::NORMAL;
        try {
            state = Color::RED + "loading command name" +
                    Color::NORMAL;
            name = FROM_JSON(j, name, std::vector<std::string>);
            if (j.contains("node")) {
                state = Color::RED + "loading nodes" +
                        Color::NORMAL;
                for (const auto &item: j.at("node")) {
                    nodes.push_back(getNodeFromJson(item, cpack));
                }
            }
            state = Color::RED + "loading start nodes" +
                    Color::NORMAL;
            auto startNodeIds = FROM_JSON(j, start, std::vector<std::string>);
            for (const auto &startNodeId: startNodeIds) {
                state = std::string(Color::RED).append("linking startNode \"")
                        .append(Color::PURPLE).append(startNodeId)
                        .append(Color::RED).append("\" to nodes")
                        .append(Color::NORMAL);
                if (startNodeId == "LF") {
                    startNodes.push_back(NodeLF::getInstance());
                    continue;
                }
                for (auto &node: nodes) {
                    if (node->id == startNodeId) {
                        startNodes.push_back(node);
                        break;
                    }
                }
            }
            if (j.contains("ast")) {
                state = Color::RED + "loading ast" +
                        Color::NORMAL;
                auto ast = FROM_JSON(j, ast, std::vector<std::vector<std::string>>);
                for (const auto &childNodes: ast) {
                    state = std::string(Color::RED).append("linking child nodes to parent node")
                            .append(Color::NORMAL);
                    if (childNodes.empty()) {
                        throw Exception::RequireParentNodeId(name);
                    }
                    auto parentNodeId = childNodes.at(0);
                    state = std::string(Color::RED).append("linking child nodes to parent node \"")
                            .append(Color::PURPLE).append(parentNodeId)
                            .append(Color::RED);
                    state.push_back('\"');
                    state.append(Color::NORMAL);
                    if (childNodes.size() == 1) {
                        throw Exception::RequireChildNodeIds(name, parentNodeId);
                    }
                    std::shared_ptr<Node::NodeBase> parentNode = nullptr;
                    for (auto &node: nodes) {
                        if (node->id == parentNodeId) {
                            parentNode = node;
                            break;
                        }
                    }
                    if (parentNode == nullptr) {
                        throw Exception::UnknownNodeId(name, parentNodeId);
                    }
                    for_each(childNodes.begin() + 1, childNodes.end(), [&](const auto &childNodeId) {
                        state = std::string(Color::RED).append("linking child node \"")
                                .append(Color::PURPLE).append(childNodeId)
                                .append(Color::RED).append("\" to parent node \"")
                                .append(Color::PURPLE).append(parentNodeId)
                                .append(Color::RED).append("\"")
                                .append(Color::NORMAL);
                        if (childNodeId == "LF") {
                            parentNode->nextNodes.push_back(Node::NodeLF::getInstance());
                            return;
                        }
                        std::shared_ptr<Node::NodeBase> childNode = nullptr;
                        for (auto &node: nodes) {
                            if (node->id == childNodeId) {
                                childNode = node;
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
        } catch (Exception::CHelperException &e) {
            throw Exception::CommandLoadFailed(state, Color::RED + e.reason());
        } catch (std::exception &e) {
            throw Exception::CommandLoadFailed(state, Color::BLUE + "[CHelper] " +
                                                      Color::RED + "Exception (" +
                                                      Color::PURPLE + "std::exception" +
                                                      Color::RED + ")" +
                                                      Color::NORMAL + " -> " +
                                                      Color::RED + e.what());
        } catch (std::string &s) {
            throw Exception::CommandLoadFailed(state, Color::BLUE + "[CHelper] " +
                                                      Color::RED + "Exception (" +
                                                      Color::PURPLE + "std::string" +
                                                      Color::RED + ")" +
                                                      Color::NORMAL + " -> " +
                                                      Color::RED + s);
        }
    }

    void NodePerCommand::toJson(nlohmann::json &j) const {
        TO_JSON(j, name);
        TO_JSON_OPTIONAL(j, description)
        TO_JSON(j, nodes);
    }

    ASTNode NodePerCommand::getASTNode(TokenReader &tokenReader, const CPack &cpack) const {
        tokenReader.push();
        //命令名字的检查
        ASTNode astNodeCommandName = getStringASTNode(tokenReader);
        const Token &token = astNodeCommandName.tokens[0];
        bool isError = astNodeCommandName.isError();
        if (!isError) {
            isError = true;
            for (const auto &item: name) {
                if (token.content == item) {
                    isError = false;
                    break;
                }
            }
        }
        if (isError) {
            tokenReader.pop();
            return ASTNode::andNode(this, {astNodeCommandName}, astNodeCommandName.tokens,
                                    ErrorReason::errorContent(astNodeCommandName.tokens, util::Format(
                                            "指令名字不匹配，找不到名为{0}的指令", token.content)));
        }
        //命令检测
        std::vector<ASTNode> childASTNodes;
        childASTNodes.reserve(startNodes.size());
        for (const auto &item: startNodes) {
            tokenReader.push();
            childASTNodes.push_back(item->getASTNodeWithNextNode(tokenReader, cpack));
            tokenReader.restore();
        }
        tokenReader.push();
        while (tokenReader.ready()) {
            if (tokenReader.read()->type == TokenType::LF) {
                break;
            }
        }
        ASTNode astNodePerCommand = ASTNode::orNode(this, childASTNodes, tokenReader.collect());
        //返回结果
        return ASTNode::andNode(this, {astNodeCommandName, astNodePerCommand}, tokenReader.collect());
    }

} // CHelper::Node