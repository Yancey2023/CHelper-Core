//
// Created by Yancey on 2023/11/7.
//

#include "CPack.h"
#include "../node/param/NodeCommand.h"
#include "../node/param/NodeNormalId.h"
#include "../node/param/NodeNamespaceId.h"

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
                std::string id = FROM_JSON(j, id, std::string);
                auto content = std::make_shared<std::vector<std::shared_ptr<NormalId>>>();
                for (const auto &item: j.at("content")) {
                    content->push_back(std::make_shared<NormalId>(item));
                }
                normalIds.emplace(std::move(id), std::move(content));
            } else if (type == "block") {
                for (const auto &item: j.at("blocks")) {
                    std::shared_ptr<BlockId> ptr = std::make_shared<BlockId>(item);
                    blockIds->push_back(ptr);
                }
            } else if (type == "item") {
                for (const auto &item: j.at("items")) {
                    std::shared_ptr<NamespaceId> ptr = std::make_shared<ItemId>(item);
                    itemIds->push_back(ptr);
                }
            } else if (type == "namespace") {
                std::string id = FROM_JSON(j, id, std::string);
                auto content = std::make_shared<std::vector<std::shared_ptr<NamespaceId>>>();
                for (const auto &item: j.at("content")) {
                    content->push_back(std::make_shared<NamespaceId>(item));
                }
                namespaceIds.emplace(std::move(id), std::move(content));
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
            commands->push_back(std::make_shared<Node::NodePerCommand>(JsonUtil::getJsonFromPath(file), *this));
        }
        Profile::pop();
        mainNode = std::make_shared<Node::NodeCommand>("MAIN_NODE", "欢迎使用命令助手(作者：Yancey)", commands);
    }

    CPack CPack::create(const std::filesystem::path &path) {
        Profile::push(ColorStringBuilder().red("start load CPack by JSON: ").purple(path.string()).build());
        CPack cpack = CPack(std::filesystem::path(path));
        Profile::pop();
        return cpack;
    }

    std::shared_ptr<Node::NodeBase> CPack::getNormalId(const std::optional<std::string> &id,
                                                       const std::optional<std::string> &description,
                                                       const std::string &key) const {
        auto it = normalIds.find(key);
        if (it == normalIds.end()) {
            return nullptr;
        }
        return std::make_shared<Node::NodeNormalId>(id, description, key, it->second);
    }

    std::shared_ptr<Node::NodeBase> CPack::getNamespaceId(const std::optional<std::string> &id,
                                                          const std::optional<std::string> &description,
                                                          const std::string &key) const {
        auto it = namespaceIds.find(key);
        if (it == namespaceIds.end()) {
            return nullptr;
        }
        return std::make_shared<Node::NodeNamespaceId>(id, description, key, it->second);
    }


} // CHelper

