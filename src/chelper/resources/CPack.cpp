//
// Created by Yancey on 2023/11/7.
//

#include "CPack.h"
#include "../node/param/NodeNormalId.h"
#include "../node/util/NodeAnd.h"

namespace CHelper {

    CODEC(RepeatData, id, breakNodes, repeatNodes)

    CPack::CPack(const std::filesystem::path &path) {
#if CHelperDebug == true
        size_t stackSize = Profile::stack.size();
#endif
        Profile::push(ColorStringBuilder().red("init codes").build());
        Node::NodeType::init();
        Profile::next(ColorStringBuilder().red("loading manifest").build());
        JsonUtil::getJsonFromFile(path / "manifest.json").get_to<Manifest>(manifest);
        Profile::next(ColorStringBuilder().red("loading id data").build());
        for (const auto &file: std::filesystem::recursive_directory_iterator(path / "id")) {
            Profile::next(ColorStringBuilder()
                                  .red("loading id data in path \"")
                                  .purple(file.path().string())
                                  .red("\"")
                                  .build());
            applyId(JsonUtil::getJsonFromFile(file));
        }
        Profile::next(ColorStringBuilder().red("loading json data").build());
        for (const auto &file: std::filesystem::recursive_directory_iterator(path / "json")) {
            Profile::next(ColorStringBuilder()
                                  .red("loading json data in path \"")
                                  .purple(file.path().string())
                                  .red("\"")
                                  .build());
            applyJson(JsonUtil::getJsonFromFile(file));
        }
        Profile::next(ColorStringBuilder().red("loading repeat data").build());
        for (const auto &file: std::filesystem::recursive_directory_iterator(path / "repeat")) {
            Profile::next(ColorStringBuilder()
                                  .red("loading repeat data in path \"")
                                  .purple(file.path().string())
                                  .red("\"")
                                  .build());
            applyRepeat(JsonUtil::getJsonFromFile(file));
        }
        Profile::next(ColorStringBuilder().red("loading commands").build());
        for (const auto &file: std::filesystem::recursive_directory_iterator(path / "command")) {
            Profile::next(ColorStringBuilder()
                                  .red("loading command in path \"")
                                  .purple(file.path().string())
                                  .red("\"")
                                  .build());
            applyCommand(JsonUtil::getJsonFromFile(file));
        }
        Profile::next(ColorStringBuilder().red("init cpack").build());
        afterApply();
        Profile::pop();
#if CHelperDebug == true
        if (HEDLEY_UNLIKELY(Profile::stack.size() != stackSize)) {
            CHELPER_WARN("error profile stack after loading cpack");
        }
#endif
    }

    CPack::CPack(const nlohmann::json &j) {
#if CHelperDebug == true
        size_t stackSize = Profile::stack.size();
#endif
        Profile::push(ColorStringBuilder().red("init codes").build());
        Node::NodeType::init();
        Profile::next(ColorStringBuilder().red("loading manifest").build());
        j.at("manifest").get_to(manifest);
        Profile::next(ColorStringBuilder().red("loading id data").build());
        for (const auto &item: j.at("id")) {
            applyId(item);
        }
        Profile::next(ColorStringBuilder().red("loading json data").build());
        for (const auto &item: j.at("json")) {
            applyJson(item);
        }
        Profile::next(ColorStringBuilder().red("loading repeat data").build());
        for (const auto &item: j.at("repeat")) {
            applyRepeat(item);
        }
        Profile::next(ColorStringBuilder().red("loading command data").build());
        for (const auto &item: j.at("command")) {
            applyCommand(item);
        }
        Profile::next(ColorStringBuilder().red("init cpack").build());
        afterApply();
        Profile::pop();
#if CHelperDebug == true
        if (HEDLEY_UNLIKELY(Profile::stack.size() != stackSize)) {
            CHELPER_WARN("error profile stack after loading cpack");
        }
#endif
    }

    CPack::CPack(BinaryReader &binaryReader) {
#if CHelperDebug == true
        size_t stackSize = Profile::stack.size();
#endif
        Profile::push(ColorStringBuilder().red("init node types").build());
        Node::NodeType::init();
        Profile::next(ColorStringBuilder().red("loading manifest").build());
        binaryReader.decode(manifest);
        Profile::next(ColorStringBuilder().red("loading normal id data").build());
        binaryReader.decode(normalIds);
        Profile::next(ColorStringBuilder().red("loading namespace id data").build());
        binaryReader.decode(namespaceIds);
        Profile::next(ColorStringBuilder().red("loading item id data").build());
        binaryReader.decode(itemIds);
        Profile::next(ColorStringBuilder().red("loading block id data").build());
        binaryReader.decode(blockIds);
        Profile::next(ColorStringBuilder().red("loading json data").build());
        Node::NodeType::canLoadNodeJson = true;
        binaryReader.decode(jsonNodes);
        Node::NodeType::canLoadNodeJson = false;
        Profile::next(ColorStringBuilder().red("loading repeat data").build());
        binaryReader.decode(repeatNodeData);
        Profile::next(ColorStringBuilder().red("loading command data").build());
        binaryReader.decode(commands);
        Profile::next(ColorStringBuilder().red("init cpack").build());
        afterApply();
        Profile::pop();
#if CHelperDebug == true
        if (HEDLEY_UNLIKELY(Profile::stack.size() != stackSize)) {
            CHELPER_WARN("error profile stack after loading cpack");
        }
#endif
    }

    void CPack::applyId(const nlohmann::json &j) {
        auto type = JsonUtil::read<std::string>(j, "type");
        if (HEDLEY_LIKELY(type == "normal")) {
            auto id = j.at("id").get<std::string>();
            auto content = std::make_shared<std::vector<std::shared_ptr<NormalId>>>();
            const auto &contentJson = j.at("content");
            content->reserve(contentJson.size());
            for (const auto &item: contentJson) {
                content->push_back(std::make_shared<NormalId>(item));
            }
            normalIds.emplace(std::move(id), std::move(content));
        } else if (HEDLEY_LIKELY(type == "namespace")) {
            auto id = j.at("id").get<std::string>();
            auto content = std::make_shared<std::vector<std::shared_ptr<NamespaceId>>>();
            const auto &contentJson = j.at("content");
            content->reserve(contentJson.size());
            for (const auto &item: contentJson) {
                content->push_back(std::make_shared<NamespaceId>(item));
            }
            namespaceIds.emplace(std::move(id), std::move(content));
        } else if (HEDLEY_LIKELY(type == "block")) {
            const auto &blocksJson = j.at("blocks");
            blockIds->reserve(blockIds->size() + blocksJson.size());
            for (const auto &item: blocksJson) {
                blockIds->push_back(std::make_shared<BlockId>(item));
            }
        } else if (HEDLEY_LIKELY(type == "item")) {
            const auto &itemsJson = j.at("items");
            itemIds->reserve(itemIds->size() + itemsJson.size());
            for (const auto &item: itemsJson) {
                itemIds->push_back(std::make_shared<ItemId>(item));
            }
        } else {
            throw Exception::UnknownIdType(type);
        }
    }

    void CPack::applyJson(const nlohmann::json &j) {
        Node::NodeType::canLoadNodeJson = true;
        jsonNodes.push_back(j);
        Node::NodeType::canLoadNodeJson = false;
    }

    void CPack::applyRepeat(const nlohmann::json &j) {
        repeatNodeData.emplace_back(j);
    }

    void CPack::applyCommand(const nlohmann::json &j) {
        commands->push_back(j);
    }

    void CPack::afterApply() {
        Profile::push(ColorStringBuilder().red("init json nodes").build());
        for (const auto &item: jsonNodes) {
            item->init(*this);
        }
        for (const auto &item: repeatNodeData) {
            std::vector<const Node::NodeBase *> content;
            content.reserve(item.repeatNodes.size());
            for (const auto &item2: item.repeatNodes) {
                std::vector<const Node::NodeBase *> perContent;
                perContent.reserve(item2.size());
                for (const auto &item3: item2) {
                    perContent.push_back(item3.get());
                }
                auto node = std::make_unique<Node::NodeAnd>(item.id, std::nullopt, std::move(perContent));
                content.push_back(node.get());
                repeatCacheNodes.push_back(std::move(node));
            }
            std::vector<const Node::NodeBase *> breakChildNodes;
            breakChildNodes.reserve(item.breakNodes.size());
            for (const auto &item2: item.breakNodes) {
                breakChildNodes.push_back(item2.get());
            }
            std::unique_ptr<Node::NodeBase> unBreakNode = std::make_unique<Node::NodeOr>(
                    item.id, std::nullopt, std::move(content), false);
            std::unique_ptr<Node::NodeBase> breakNode = std::make_unique<Node::NodeAnd>(
                    item.id, std::nullopt, std::move(breakChildNodes));
            repeatNodes.emplace_back(unBreakNode.get(), breakNode.get());
            repeatCacheNodes.push_back(std::move(unBreakNode));
            repeatCacheNodes.push_back(std::move(breakNode));
        }
        for (const auto &item: *commands) {
            Profile::next(ColorStringBuilder().red("init command: \"").purple(StringUtil::join(",", item->name)).red("\"").build());
            item->init(*this);
        }
        Profile::next(ColorStringBuilder().red("sort command nodes").build());
        std::sort(commands->begin(), commands->end(),
                  [](const auto &item1, const auto &item2) {
                      return ((Node::NodePerCommand *) item1.get())->name[0] <
                             ((Node::NodePerCommand *) item2.get())->name[0];
                  });
        Profile::next(ColorStringBuilder().red("create main node").build());
        mainNode = std::make_unique<Node::NodeCommand>("MAIN_NODE", "欢迎使用命令助手(作者：Yancey)", commands.get());
        Profile::pop();
    }

    std::unique_ptr<CPack> CPack::createByDirectory(const std::filesystem::path &path) {
        Profile::push(ColorStringBuilder().red("start load CPack by DIRECTORY: ").purple(path.string()).build());
        std::unique_ptr<CPack> cpack = std::make_unique<CPack>(path);
        Profile::pop();
        return cpack;
    }

    std::unique_ptr<CPack> CPack::createByJson(const nlohmann::json &j) {
        Profile::push(ColorStringBuilder().red("start load CPack by JSON").build());
        std::unique_ptr<CPack> cpack = std::make_unique<CPack>(j);
        Profile::pop();
        return cpack;
    }

    std::unique_ptr<CPack> CPack::createByBinary(BinaryReader &binaryReader) {
        Profile::push(ColorStringBuilder().red("start load CPack by binary").build());
        std::unique_ptr<CPack> cpack = std::make_unique<CPack>(binaryReader);
        Profile::pop();
        return cpack;
    }

    void CPack::writeJsonToDirectory(const std::filesystem::path &path) const {
        JsonUtil::writeJsonToFile(path / "manifest.json", manifest);
        for (const auto &item: normalIds) {
            nlohmann::json j;
            j["id"] = item.first;
            j["type"] = "normal";
            j["content"] = item.second;
            JsonUtil::writeJsonToFile(path / "id" / (item.first + ".json"), j);
        }
        for (const auto &item: namespaceIds) {
            nlohmann::json j;
            j["id"] = item.first;
            j["type"] = "namespace";
            j["content"] = item.second;
            JsonUtil::writeJsonToFile(path / "id" / (item.first + ".json"), j);
        }
        {
            nlohmann::json j;
            j["type"] = "item";
            j["items"] = *itemIds;
            JsonUtil::writeJsonToFile(path / "id" / "items.json", j);
        }
        {
            nlohmann::json j;
            j["type"] = "block";
            j["blocks"] = *blockIds;
            JsonUtil::writeJsonToFile(path / "id" / "blocks.json", j);
        }
        for (const auto &item: jsonNodes) {
            JsonUtil::writeJsonToFile(path / "json" / (item->id.value() + ".json"), item);
        }
        for (const auto &item: repeatNodes) {
            nlohmann::json j;
            j["id"] = item.first->id.value();
            nlohmann::json jsonContent;
            for (const auto &item2: ((Node::NodeOr *) item.first)->childNodes) {
                nlohmann::json temp1;
                for (const auto &item3: ((Node::NodeAnd *) item2)->childNodes) {
                    nlohmann::json temp2 = *item3;
                    temp2["type"] = item3->getNodeType()->nodeName;
                    temp1.push_back(std::move(temp2));
                }
                jsonContent.push_back(std::move(temp1));
            }
            j["content"] = std::move(jsonContent);
            nlohmann::json jsonBreak;
            for (const auto &item2: ((Node::NodeAnd *) item.second)->childNodes) {
                nlohmann::json temp = *item2;
                temp["type"] = item2->getNodeType()->nodeName;
                jsonBreak.push_back(std::move(temp));
            }
            j["break"] = std::move(jsonBreak);
            JsonUtil::writeJsonToFile(path / "repeat" / (item.first->id.value() + ".json"), j);
        }
        for (const auto &item: *commands) {
            JsonUtil::writeJsonToFile(
                    path / "command" / (((Node::NodePerCommand *) item.get())->name[0] + ".json"),
                    item);
        }
    }

    nlohmann::json CPack::toJson() const {
        nlohmann::json result;
        result["manifest"] = manifest;
        nlohmann::json idJson;
        for (const auto &item: normalIds) {
            nlohmann::json j;
            j["id"] = item.first;
            j["type"] = "normal";
            j["content"] = *item.second;
            idJson.push_back(std::move(j));
        }
        for (const auto &item: namespaceIds) {
            nlohmann::json j;
            j["id"] = item.first;
            j["type"] = "namespace";
            j["content"] = *item.second;
            idJson.push_back(std::move(j));
        }
        {
            nlohmann::json j;
            j["type"] = "item";
            j["items"] = *itemIds;
            idJson.push_back(std::move(j));
        }
        {
            nlohmann::json j;
            j["type"] = "block";
            j["blocks"] = *blockIds;
            idJson.push_back(std::move(j));
        }
        result["id"] = std::move(idJson);
        result["json"] = jsonNodes;
        nlohmann::json repeatJson;
        for (const auto &item: repeatNodes) {
            nlohmann::json j;
            j["id"] = item.first->id.value();
            nlohmann::json jsonContent;
            for (const auto &item2: ((Node::NodeOr *) item.first)->childNodes) {
                nlohmann::json temp1;
                for (const auto &item3: ((Node::NodeAnd *) item2)->childNodes) {
                    nlohmann::json temp2 = *item3;
                    temp2["type"] = item3->getNodeType()->nodeName;
                    temp1.push_back(std::move(temp2));
                }
                jsonContent.push_back(std::move(temp1));
            }
            j["content"] = std::move(jsonContent);
            nlohmann::json jsonBreak;
            for (const auto &item2: ((Node::NodeAnd *) item.second)->childNodes) {
                nlohmann::json temp;
                temp["type"] = item2->getNodeType()->nodeName;
                jsonBreak.push_back(std::move(temp));
            }
            j["break"] = std::move(jsonBreak);
            repeatJson.push_back(std::move(j));
        }
        result["repeat"] = std::move(repeatJson);
        result["command"] = *commands;
        return result;
    }

    void CPack::writeJsonToFile(const std::filesystem::path &path) const {
        JsonUtil::writeJsonToFile(path, toJson());
    }

    void CPack::writeBsonToFile(const std::filesystem::path &path) const {
        JsonUtil::writeBsonToFile(path, toJson());
    }

    void CPack::writeBinToFile(const std::filesystem::path &path) const {
        std::filesystem::create_directories(path.parent_path());
        Profile::push(ColorStringBuilder()
                              .red("writing binary cpack to file: ")
                              .purple(path.string())
                              .build());
        std::ofstream f(path, std::ios::binary);
        BinaryWriter binaryWriter(true, f);

        //manifest
        binaryWriter.encode(manifest);
        //normal id
        binaryWriter.encode(normalIds);
        //namespace id
        binaryWriter.encode(namespaceIds);
        //item id
        binaryWriter.encode(itemIds);
        //block id
        binaryWriter.encode(blockIds);
        //json node
        binaryWriter.encode(jsonNodes);
        //repeat node
        binaryWriter.encode(repeatNodeData);
        //command
        binaryWriter.encode(commands);

        f.close();
        Profile::pop();
    }

    std::shared_ptr<std::vector<std::shared_ptr<NormalId>>>
    CPack::getNormalId(const std::string &key) const {
        auto it = normalIds.find(key);
        if (HEDLEY_UNLIKELY(it == normalIds.end())) {
#if CHelperDebug == true
            CHELPER_WARN("fail to find normal ids by key: \"" + key + '\"');
#endif
            return nullptr;
        }
        return it->second;
    }

    std::shared_ptr<std::vector<std::shared_ptr<NamespaceId>>>
    CPack::getNamespaceId(const std::string &key) const {
        auto it = namespaceIds.find(key);
        if (HEDLEY_UNLIKELY(it == namespaceIds.end())) {
#if CHelperDebug == true
            CHELPER_WARN("fail to find namespace ids by key: \"" + key + '\"');
#endif
            return nullptr;
        }
        return it->second;
    }

}// namespace CHelper
