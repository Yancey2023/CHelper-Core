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
            jsonNodes.push_back(std::make_unique<Node::NodeJsonElement>(JsonUtil::getJsonFromPath(file), *this));
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
            std::vector<const Node::NodeBase *> content;
            for (const auto &item: j.at("contents")) {
                std::vector<std::unique_ptr<Node::NodeBase>> perContentData;
                std::vector<const Node::NodeBase *> perContent;
                for (const auto &item2: item) {
                    auto node = Node::NodeBase::getNodeFromJson(item2, *this);
                    perContent.push_back(node.get());
                    repeatNodeDatas.push_back(std::move(node));
                }
                auto node = std::make_unique<Node::NodeAnd>(id, std::nullopt, perContent);
                content.push_back(node.get());
                repeatNodeDatas.push_back(std::move(node));
            }
            std::unique_ptr<Node::NodeBase> unBreakNode = std::make_unique<Node::NodeOr>(
                    id, std::nullopt, content, false);
            std::unique_ptr<Node::NodeBase> breakNode = Node::NodeBase::getNodeFromJson(j.at("break"), *this);
            repeatNodes.emplace_back(unBreakNode.get(), breakNode.get());
            repeatNodeDatas.push_back(std::move(unBreakNode));
            repeatNodeDatas.push_back(std::move(breakNode));
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
            commands->push_back(std::make_unique<Node::NodePerCommand>(JsonUtil::getJsonFromPath(file), *this));
        }
        Profile::pop();
        std::vector<const Node::NodeBase *> commands1;
        for (const auto &item: *commands) {
            commands1.push_back(item.get());
        }
        mainNode = new Node::NodeCommand("MAIN_NODE", "欢迎使用命令助手(作者：Yancey)", commands1);
#if CHelperDebug == true
        if (Profile::stack.size() != stackSize) {
            CHELPER_WARN("error profile stack after loading cpack");
        }
#endif
    }

    CPack::~CPack() {
        delete mainNode;
    }

    std::unique_ptr<CPack> CPack::create(const std::filesystem::path &path) {
        //因为去除了CPack的复制，所以这里Profile的pop写在了构造函数里面
        Profile::push(ColorStringBuilder().red("start load CPack by JSON: ").purple(path.string()).build());
        std::unique_ptr<CPack> cpack = std::make_unique<CPack>(std::filesystem::path(path));
        Profile::pop();
        return cpack;
    }

    std::shared_ptr<std::vector<std::shared_ptr<NormalId>>>
    CPack::getNormalId(const std::string &key) const {
        auto it = normalIds.find(key);
        if (it == normalIds.end()) {
            return nullptr;
        }
        return it->second;
    }

    std::shared_ptr<std::vector<std::shared_ptr<NamespaceId>>>
    CPack::getNamespaceId(const std::string &key) const {
        auto it = namespaceIds.find(key);
        if (it == namespaceIds.end()) {
            return nullptr;
        }
        return it->second;
    }

} // CHelper

