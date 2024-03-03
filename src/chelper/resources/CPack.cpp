//
// Created by Yancey on 2023/11/7.
//

#include "CPack.h"
#include "../node/param/NodeCommand.h"
#include "../node/param/NodeNormalId.h"
#include "../node/param/NodeNamespaceId.h"
#include "../node/util/NodeList.h"
#include "../node/json/NodeJsonList.h"
#include "../node/json/NodeJsonObject.h"
#include "../node/json/NodeJsonElement.h"
#include "../node/util/NodeAnd.h"

namespace CHelper {

    CPack::CPack(const std::filesystem::path &path)
            : manifest(JsonUtil::getJsonFromPath(path / "manifest.json").get<Manifest>()) {
#if CHelperDebug == true
        size_t stackSize = Profile::stack.size();
#endif
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
                auto id = j.at("id").get<std::string>();
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
                auto id = j.at("id").get<std::string>();
                auto content = std::make_shared<std::vector<std::shared_ptr<NamespaceId>>>();
                for (const auto &item: j.at("content")) {
                    content->push_back(std::make_shared<NamespaceId>(item));
                }
                namespaceIds.emplace(std::move(id), std::move(content));
            } else {
                throw Exception::UnknownIdType(file.path().filename().string(), type);
            }
        }
        Profile::next(ColorStringBuilder().red("loading json data").build());
        Node::NodeType::canLoadNodeJson = true;
        for (const auto &file: std::filesystem::recursive_directory_iterator(path / "json")) {
            Profile::next(ColorStringBuilder()
                                  .red("loading json data in path \"")
                                  .purple(file.path().string())
                                  .red("\"")
                                  .build());
            jsonNodes.push_back(std::make_shared<Node::NodeJsonElement>(JsonUtil::getJsonFromPath(file), *this));
        }
        Profile::next(ColorStringBuilder().red("loading repeat data").build());
        for (const auto &file: std::filesystem::recursive_directory_iterator(path / "repeat")) {
            Profile::next(ColorStringBuilder()
                                  .red("loading repeat data in path \"")
                                  .purple(file.path().string())
                                  .red("\"")
                                  .build());
            nlohmann::json j = JsonUtil::getJsonFromPath(file);
            Profile::push(ColorStringBuilder().red("loading repeat data id").build());
            auto id = JsonUtil::fromJson<std::string>(j, "id");
            Profile::next(ColorStringBuilder().red("loading contents").build());
            auto content = std::make_shared<std::vector<std::shared_ptr<Node::NodeBase>>>();
            for (const auto &item: j.at("contents")) {
                auto perContent = std::make_shared<std::vector<std::shared_ptr<Node::NodeBase>>>();
                for (const auto &item2: item) {
                    perContent->push_back(Node::NodeBase::getNodeFromJson(item2, *this));
                }
                content->push_back(std::make_shared<Node::NodeAnd>(id, std::nullopt, perContent));
            }
            auto breakNode = Node::NodeBase::getNodeFromJson(j.at("break"), *this);
            repeatNodes.emplace_back(std::make_shared<Node::NodeOr>(id, std::nullopt, content, false), breakNode);
            Profile::pop();
        }
        Node::NodeType::canLoadNodeJson = false;
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
#if CHelperDebug == true
        if (Profile::stack.size() != stackSize) {
            CHELPER_WARN("error profile stack after loading cpack");
        }
#endif
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
        return std::make_shared<Node::NodeNormalId>(id, description, key, true, it->second);
    }

    std::shared_ptr<Node::NodeBase> CPack::getNamespaceId(const std::optional<std::string> &id,
                                                          const std::optional<std::string> &description,
                                                          const std::string &key) const {
        auto it = namespaceIds.find(key);
        if (it == namespaceIds.end()) {
            return nullptr;
        }
        return std::make_shared<Node::NodeNamespaceId>(id, description, key, true, it->second);
    }

} // CHelper

