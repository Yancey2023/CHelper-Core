//
// Created by Yancey666 on 2023/11/7.
//

#include "CPack.h"

namespace CHelper {

    CPack::CPack(const std::filesystem::path &path)
            : manifest(JsonUtil::getJsonFromPath(path / "manifest.json").get<Manifest>()) {
        std::string state = Color::RED + "unknown state" +
                            Color::NORMAL;
        try {
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
                    std::vector<std::shared_ptr<NormalId>> content;
                    for (const auto &item: j.at("content")) {
                        content.push_back(std::make_shared<NormalId>(item));
                    }
                    normalIds.emplace(
                            FROM_JSON(j, id, std::string),
                            content
                    );
                } else if (type == "block") {
                    std::vector<std::shared_ptr<NamespaceId>> blocks;
                    for (const auto &item: j.at("blocks")) {
                        std::shared_ptr<BlockId> ptr = std::make_shared<BlockId>(item);
                        blockIds.push_back(ptr);
                        blocks.push_back(ptr);
                    }
                    namespaceIds.emplace("blocks", blocks);
                } else if (type == "item") {
                    std::vector<std::shared_ptr<NamespaceId>> items;
                    for (const auto &item: j.at("items")) {
                        std::shared_ptr<ItemId> ptr = std::make_shared<ItemId>(item);
                        itemIds.push_back(ptr);
                        items.push_back(ptr);
                    }
                    namespaceIds.emplace("items", items);
                } else if (type == "namespace") {
                    std::vector<std::shared_ptr<NamespaceId>> content;
                    for (const auto &item: j.at("content")) {
                        content.push_back(std::make_shared<NamespaceId>(item));
                    }
                    namespaceIds.emplace(
                            FROM_JSON(j, id, std::string),
                            content
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
                commands.push_back(std::make_shared<Node::NodePerCommand>(JsonUtil::getJsonFromPath(file), *this));
            }
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

