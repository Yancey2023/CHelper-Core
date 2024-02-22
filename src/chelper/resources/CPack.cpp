//
// Created by Yancey666 on 2023/11/7.
//

#include "CPack.h"

namespace CHelper {

    CPack::CPack(const std::filesystem::path &path)
            : manifest(JsonUtil::getJsonFromPath(path / "manifest.json").get<Manifest>()) {
        Profile::push(ColorStringBuilder().red("list path of id").build());
        for (const auto &file: std::filesystem::recursive_directory_iterator(path / "id")) {
            Profile::next(ColorStringBuilder()
                                  .red("loading id in path \"")
                                  .purple(file.path().string())
                                  .red("\"")
                                  .build());
            nlohmann::json j = JsonUtil::getJsonFromPath(file);
            std::string type = FROM_JSON(j, type, std::string);
            Profile::next(ColorStringBuilder()
                                  .red("loading ")
                                  .purple(type)
                                  .red(" id in path \"")
                                  .purple(file.path().string())
                                  .red("\"").build());
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
        Profile::next(ColorStringBuilder().red("loading commands").build());
        for (const auto &file: std::filesystem::recursive_directory_iterator(path / "command")) {
            Profile::next(ColorStringBuilder()
                                  .red("loading command in path \"")
                                  .purple(file.path().string())
                                  .red("\"")
                                  .build());
            commands.push_back(std::make_shared<Node::NodePerCommand>(JsonUtil::getJsonFromPath(file), *this));
        }
        Profile::pop();
    }

    CPack CPack::create(const std::filesystem::path &path) {
        Profile::push(ColorStringBuilder().red("start load CPack by JSON: ").purple(path.string()).build());
        CPack cpack = CPack(std::filesystem::path(path));
        Profile::pop();
        return cpack;
    }


} // CHelper

