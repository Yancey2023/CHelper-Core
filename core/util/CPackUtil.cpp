//
// Created by Yancey666 on 2023/11/30.
//

#include "CPackUtil.h"
#include "..\resources\command\type\NodeType.h"
#include "..\resources\command\node\NodeLF.h"
#include "Exception.h"

namespace CHelper {
    namespace Node {
        std::shared_ptr<NodeBase> getNodeFromJson(const nlohmann::json &j,
                                                  const CPack &cpack) {
            std::string state = Color::RED + "unknown state" +
                                Color::NORMAL;
            try {
                state = Color::RED + "loading type" +
                        Color::NORMAL;
                std::string type = FROM_JSON(j, type, std::string);
                state = Color::RED + "loading node " +
                        Color::PURPLE + type +
                        Color::NORMAL;
                for (const auto &item: NodeType::NODE_TYPES) {
                    if (item->nodeName == type) {
                        return item->createNodeByJson(j, cpack);
                    }
                }
                throw Exception::UnknownNodeType(type);
            } catch (Exception::CHelperException &e) {
                throw Exception::NodeLoadFailed(state, Color::RED + e.reason());
            } catch (std::exception &e) {
                throw Exception::NodeLoadFailed(state, Color::BLUE + "[CHelper] " +
                                                       Color::RED + "Exception (" +
                                                       Color::PURPLE + "std::exception" +
                                                       Color::RED + ")" +
                                                       Color::NORMAL + " -> " +
                                                       Color::RED + e.what());
            } catch (std::string &s) {
                throw Exception::NodeLoadFailed(state, Color::BLUE + "[CHelper] " +
                                                       Color::RED + "Exception (" +
                                                       Color::PURPLE + "std::string" +
                                                       Color::RED + ")" +
                                                       Color::NORMAL + " -> " +
                                                       Color::RED + s);
            }
        }
    } // Node

    Command getCommandFromJson(const nlohmann::json &j,
                               const CPack &cpack) {
        std::vector<std::string> name = FROM_JSON(j, name, std::vector<std::string>);
        std::optional<std::string> description = FROM_JSON_OPTIONAL(j, description, std::string);
        std::string state = Color::RED + "unknown state" +
                            Color::NORMAL;
        try {
            std::vector<std::shared_ptr<Node::NodeBase>> nodes;
            if (j.contains("node")) {
                state = Color::RED + "loading nodes" +
                        Color::NORMAL;
                for (const auto &item: j.at("node")) {
                    nodes.push_back(Node::getNodeFromJson(item, cpack));
                }
            }
            state = Color::RED + "loading start nodes" +
                    Color::NORMAL;
            std::vector<std::shared_ptr<Node::NodeBase>> startNodes;
            auto startNodeIds = FROM_JSON(j, start, std::vector<std::string>);
            for (const auto &startNodeId: startNodeIds) {
                state = std::string(Color::RED).append("linking startNode \"")
                        .append(Color::PURPLE).append(startNodeId)
                        .append(Color::RED).append("\" to nodes")
                        .append(Color::NORMAL);
                if (startNodeId == "LF") {
                    startNodes.push_back(Node::NodeLF::getInstance());
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
                            parentNode->nextNodes.insert(Node::NodeLF::getInstance());
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
                        parentNode->nextNodes.insert(childNode);
                    });
                }
            }
            return {name, description, startNodes};
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

    CPack getCPack(const std::filesystem::path &path) {
        std::string state = Color::RED + "unknown state" +
                            Color::NORMAL;
        try {
            state = Color::RED + "loading manifest" +
                    Color::NORMAL;
            CPack cpack = CPack(JsonUtil::getJsonFromPath(path / "manifest.json").get<Manifest>());
            state = Color::RED + "list path of id" +
                    Color::NORMAL;
            for (const auto &file: std::filesystem::recursive_directory_iterator(path / "id")) {
                state = std::string().append(Color::RED).append("loading id in path \"")
                        .append(Color::PURPLE).append(file.path().string())
                        .append(Color::RED).append("\"")
                        .append(Color::NORMAL);
                nlohmann::json j = JsonUtil::getJsonFromPath(file);
                std::string type = FROM_JSON(j, type, std::string);
                state = std::string().append(Color::RED).append("loading ")
                        .append(Color::PURPLE).append(type)
                        .append(Color::RED).append(" id in path \"")
                        .append(Color::PURPLE).append(file.path().string())
                        .append(Color::RED).append("\"")
                        .append(Color::NORMAL);
                if (type == "normal") {
                    cpack.normalIds.emplace(
                            FROM_JSON(j, id, std::string),
                            FROM_JSON(j, content, std::vector<NormalId>)
                    );
                } else if (type == "block") {
                    cpack.blockIds = FROM_JSON(j, blocks, std::vector<BlockId>);
                } else if (type == "item") {
                    cpack.itemIds = FROM_JSON(j, items, std::vector<ItemId>);
                } else if (type == "namespace") {
                    cpack.namespaceIds.emplace(
                            FROM_JSON(j, id, std::string),
                            FROM_JSON(j, content, std::vector<NamespaceId>)
                    );
                } else {
                    throw Exception::UnknownIdType(file.path().filename().string(), type);
                }
            }
            state = Color::PURPLE + "loading commands" +
                    Color::NORMAL;
            for (const auto &file: std::filesystem::recursive_directory_iterator(path / "command")) {
                state = std::string().append(Color::RED).append("loading command in path \"")
                        .append(Color::PURPLE).append(file.path().string())
                        .append(Color::RED).append("\"")
                        .append(Color::NORMAL);
                cpack.commands.push_back(getCommandFromJson(JsonUtil::getJsonFromPath(file), cpack));
            }
            return cpack;
        } catch (Exception::CHelperException &e) {
            throw Exception::CPackLoadFailed(state, Color::RED + e.reason());
        } catch (std::exception &e) {
            throw Exception::CPackLoadFailed(state, Color::BLUE + "[CHelper] " +
                                                    Color::RED + "Exception (" +
                                                    Color::PURPLE + "std::exception" +
                                                    Color::RED + ")" +
                                                    Color::NORMAL + " -> " +
                                                    Color::RED + e.what());
        } catch (std::string &s) {
            throw Exception::CPackLoadFailed(state, Color::BLUE + "[CHelper] " +
                                                    Color::RED + "Exception (" +
                                                    Color::PURPLE + "std::string" +
                                                    Color::RED + ")" +
                                                    Color::NORMAL + " -> " +
                                                    Color::RED + s);
        }

    }

} // CHelper