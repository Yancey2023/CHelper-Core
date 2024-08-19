//
// Created by Yancey on 2023/11/7.
//

#include "CPack.h"
#include "../node/param/NodeNormalId.h"
#include "../node/util/NodeAnd.h"

namespace CHelper {

    CODEC(RepeatData, id, breakNodes, repeatNodes, isEnd)

#if CHelperOnlyReadBinary != true
    CPack::CPack(const std::filesystem::path &path) {
#if CHelperDebug == true
        size_t stackSize = Profile::stack.size();
#endif
        Profile::push("init codes");
        Node::NodeType::init();
        Profile::next("loading manifest");
        JsonUtil::getJsonFromFile(path / "manifest.json").get_to<Manifest>(manifest);
        Profile::next("loading id data");
        for (const auto &file: std::filesystem::recursive_directory_iterator(path / "id")) {
            Profile::next(R"(loading id data in path "{}")", file.path().wstring());
            applyId(JsonUtil::getJsonFromFile(file));
        }
        Profile::next("loading json data");
        Node::NodeType::currentCreateStage = Node::NodeCreateStage::JSON_NODE;
        for (const auto &file: std::filesystem::recursive_directory_iterator(path / "json")) {
            Profile::next(R"(loading json data in path "{}")", file.path().wstring());
            applyJson(JsonUtil::getJsonFromFile(file));
        }
        Profile::next("loading repeat data");
        Node::NodeType::currentCreateStage = Node::NodeCreateStage::REPEAT_NODE;
        for (const auto &file: std::filesystem::recursive_directory_iterator(path / "repeat")) {
            Profile::next(R"(loading repeat data in path "{}")", file.path().wstring());
            applyRepeat(JsonUtil::getJsonFromFile(file));
        }
        Profile::next("loading commands");
        Node::NodeType::currentCreateStage = Node::NodeCreateStage::COMMAND_PARAM_NODE;
        for (const auto &file: std::filesystem::recursive_directory_iterator(path / "command")) {
            Profile::next(R"(loading command in path "{}")", file.path().wstring());
            applyCommand(JsonUtil::getJsonFromFile(file));
        }
        Profile::next("init cpack");
        Node::NodeType::currentCreateStage = Node::NodeCreateStage::NONE;
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
        Profile::push("init codes");
        Node::NodeType::init();
        Profile::next("loading manifest");
        j.at("manifest").get_to(manifest);
        Profile::next("loading id data");
        for (const auto &item: j.at("id")) {
            applyId(item);
        }
        Profile::next("loading json data");
        Node::NodeType::currentCreateStage = Node::NodeCreateStage::JSON_NODE;
        for (const auto &item: j.at("json")) {
            applyJson(item);
        }
        Profile::next("loading repeat data");
        Node::NodeType::currentCreateStage = Node::NodeCreateStage::REPEAT_NODE;
        for (const auto &item: j.at("repeat")) {
            applyRepeat(item);
        }
        Profile::next("loading command data");
        Node::NodeType::currentCreateStage = Node::NodeCreateStage::COMMAND_PARAM_NODE;
        for (const auto &item: j.at("command")) {
            applyCommand(item);
        }
        Profile::next("init cpack");
        Node::NodeType::currentCreateStage = Node::NodeCreateStage::NONE;
        afterApply();
        Profile::pop();
#if CHelperDebug == true
        if (HEDLEY_UNLIKELY(Profile::stack.size() != stackSize)) {
            CHELPER_WARN("error profile stack after loading cpack");
        }
#endif
    }
#endif

    CPack::CPack(BinaryReader &binaryReader) {
#if CHelperDebug == true
        size_t stackSize = Profile::stack.size();
#endif
        Profile::push("init node types");
        Node::NodeType::init();
        Profile::next("loading manifest");
        binaryReader.decode(manifest);
        Profile::next("loading normal id data");
        binaryReader.decode(normalIds);
        Profile::next("loading namespace id data");
        binaryReader.decode(namespaceIds);
        Profile::next("loading item id data");
        size_t itemIdSize = binaryReader.readSize();
        for (int i = 0; i < itemIdSize; ++i) {
            itemIds->push_back(binaryReader.read<std::shared_ptr<ItemId>>());
        }
        Profile::next("loading block id data");
        size_t blockIdSize = binaryReader.readSize();
        for (int i = 0; i < blockIdSize; ++i) {
            blockIds->push_back(binaryReader.read<std::shared_ptr<BlockId>>());
        }
        Profile::next("loading json data");
        Node::NodeType::currentCreateStage = Node::NodeCreateStage::JSON_NODE;
        binaryReader.decode(jsonNodes);
        Profile::next("loading repeat data");
        Node::NodeType::currentCreateStage = Node::NodeCreateStage::REPEAT_NODE;
        binaryReader.decode(repeatNodeData);
        Profile::next("loading command data");
        Node::NodeType::currentCreateStage = Node::NodeCreateStage::COMMAND_PARAM_NODE;
        binaryReader.decode(commands);
        Profile::next("init cpack");
        Node::NodeType::currentCreateStage = Node::NodeCreateStage::NONE;
        afterApply();
        Profile::pop();
#if CHelperDebug == true
        if (HEDLEY_UNLIKELY(Profile::stack.size() != stackSize)) {
            CHELPER_WARN("error profile stack after loading cpack");
        }
#endif
    }

#if CHelperOnlyReadBinary != true
    void CPack::applyId(const nlohmann::json &j) {
        auto type = JsonUtil::read<std::wstring>(j, "type");
        if (HEDLEY_LIKELY(type == L"normal")) {
            auto id = JsonUtil::read<std::wstring>(j, "id");
            auto content = std::make_shared<std::vector<std::shared_ptr<NormalId>>>();
            const auto &contentJson = j.at("content");
            content->reserve(contentJson.size());
            for (const auto &item: contentJson) {
                content->push_back(std::make_shared<NormalId>(item));
            }
            normalIds.emplace(std::move(id), std::move(content));
        } else if (HEDLEY_LIKELY(type == L"namespace")) {
            auto id = JsonUtil::read<std::wstring>(j, "id");
            auto content = std::make_shared<std::vector<std::shared_ptr<NamespaceId>>>();
            const auto &contentJson = j.at("content");
            content->reserve(contentJson.size());
            for (const auto &item: contentJson) {
                content->push_back(std::make_shared<NamespaceId>(item));
            }
            namespaceIds.emplace(std::move(id), std::move(content));
        } else if (HEDLEY_LIKELY(type == L"block")) {
            const auto &blocksJson = j.at("blocks");
            blockIds->reserve(blockIds->size() + blocksJson.size());
            for (const auto &item: blocksJson) {
                blockIds->push_back(std::make_shared<BlockId>(item));
            }
        } else if (HEDLEY_LIKELY(type == L"item")) {
            const auto &itemsJson = j.at("items");
            itemIds->reserve(itemIds->size() + itemsJson.size());
            for (const auto &item: itemsJson) {
                itemIds->push_back(std::make_shared<ItemId>(item));
            }
        } else {
            Profile::push("unknown id type -> {}", type);
            throw std::runtime_error("unknown id type");
        }
    }

    void CPack::applyJson(const nlohmann::json &j) {
        jsonNodes.push_back(j);
    }

    void CPack::applyRepeat(const nlohmann::json &j) {
        repeatNodeData.emplace_back(j);
    }

    void CPack::applyCommand(const nlohmann::json &j) const {
        commands->push_back(j);
    }
#endif

    void CPack::afterApply() {
        // json nodes
        Profile::push("init json nodes");
        for (const auto &item: jsonNodes) {
            item->init(*this);
        }
        // repeat nodes
        Profile::next("init repeat nodes");
        for (const auto &item: repeatNodeData) {
            if (HEDLEY_UNLIKELY(item.repeatNodes.size() != item.isEnd.size())) {
                Profile::push("checking repeat node: {}", item.id);
                throw std::runtime_error("fail to check repeat id because repeatNodes size not equal isEnd size");
            }
        }
        // command param nodes
        for (const auto &item: repeatNodeData) {
            std::vector<const Node::NodeBase *> content;
            content.reserve(item.repeatNodes.size());
            for (const auto &item2: item.repeatNodes) {
                std::vector<const Node::NodeBase *> perContent;
                perContent.reserve(item2.size());
                for (const auto &item3: item2) {
                    perContent.push_back(item3.get());
                }
                auto node = std::make_unique<Node::NodeAnd>(item.id, std::nullopt, Node::WhitespaceMode::NORMAL, std::move(perContent));
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
                    item.id, std::nullopt, Node::WhitespaceMode::NORMAL, std::move(breakChildNodes));
            std::unique_ptr<Node::NodeBase> orNode = std::make_unique<Node::NodeOr>(
                    L"NODE_REPEAT", L"命令重复部分",
                    std::vector<const Node::NodeBase *>{unBreakNode.get(), breakNode.get()},
                    false);
            repeatNodes.emplace(item.id, std::make_pair(&item, orNode.get()));
            repeatCacheNodes.push_back(std::move(unBreakNode));
            repeatCacheNodes.push_back(std::move(breakNode));
            repeatCacheNodes.push_back(std::move(orNode));
        }
        for (const auto &item: repeatNodeData) {
            for (const auto &item2: item.repeatNodes) {
                for (const auto &item3: item2) {
                    item3->init(*this);
                }
            }
            for (const auto &item2: item.breakNodes) {
                item2->init(*this);
            }
        }
        for (const auto &item: *commands) {
            Profile::next(R"(init command: "{}")", StringUtil::join(L",", item->name));
            item->init(*this);
        }
        Profile::next("sort command nodes");
        std::sort(commands->begin(), commands->end(),
                  [](const auto &item1, const auto &item2) {
                      return ((Node::NodePerCommand *) item1.get())->name[0] <
                             ((Node::NodePerCommand *) item2.get())->name[0];
                  });
        Profile::next("create main node");
        mainNode = std::make_unique<Node::NodeCommand>(L"MAIN_NODE", L"欢迎使用命令助手(作者：Yancey)", commands.get());
        Profile::pop();
    }

#if CHelperOnlyReadBinary != true
    std::unique_ptr<CPack> CPack::createByDirectory(const std::filesystem::path &path) {
        Profile::push("start load CPack by DIRECTORY: {}", path.wstring());
        std::unique_ptr<CPack> cpack = std::make_unique<CPack>(path);
        Profile::pop();
        return cpack;
    }

    std::unique_ptr<CPack> CPack::createByJson(const nlohmann::json &j) {
        Profile::push("start load CPack by JSON");
        std::unique_ptr<CPack> cpack = std::make_unique<CPack>(j);
        Profile::pop();
        return cpack;
    }
#endif

    std::unique_ptr<CPack> CPack::createByBinary(BinaryReader &binaryReader) {
        Profile::push("start load CPack by binary");
        std::unique_ptr<CPack> cpack = std::make_unique<CPack>(binaryReader);
        Profile::pop();
        return cpack;
    }

#if CHelperOnlyReadBinary != true
    void CPack::writeJsonToDirectory(const std::filesystem::path &path) const {
        JsonUtil::writeJsonToFile(path / "manifest.json", manifest);
        for (const auto &item: normalIds) {
            nlohmann::json j;
            j["id"] = item.first;
            j["type"] = "normal";
            j["content"] = item.second;
            JsonUtil::writeJsonToFile(path / "id" / (item.first + L".json"), j);
        }
        for (const auto &item: namespaceIds) {
            nlohmann::json j;
            j["id"] = item.first;
            j["type"] = "namespace";
            j["content"] = item.second;
            JsonUtil::writeJsonToFile(path / "id" / (item.first + L".json"), j);
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
            JsonUtil::writeJsonToFile(path / "json" / (item->id.value() + L".json"), item);
        }
        for (const auto &item: repeatNodeData) {
            JsonUtil::writeJsonToFile(path / "repeat" / (item.id + L".json"), item);
        }
        for (const auto &item: *commands) {
            JsonUtil::writeJsonToFile(
                    path / "command" / (((Node::NodePerCommand *) item.get())->name[0] + L".json"),
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
            j["content"] = item.second;
            idJson.push_back(std::move(j));
        }
        for (const auto &item: namespaceIds) {
            nlohmann::json j;
            j["id"] = item.first;
            j["type"] = "namespace";
            j["content"] = item.second;
            idJson.push_back(std::move(j));
        }
        {
            nlohmann::json j;
            j["id"] = "items";
            j["type"] = "item";
            nlohmann::json items;
            for (const auto &item: *itemIds) {
                items.push_back(std::static_pointer_cast<ItemId>(item));
            }
            j["items"] = items;
            idJson.push_back(std::move(j));
        }
        {
            nlohmann::json j;
            j["id"] = "blocks";
            j["type"] = "block";
            nlohmann::json blocks;
            for (const auto &item: *blockIds) {
                blocks.push_back(std::static_pointer_cast<BlockId>(item));
            }
            j["blocks"] = blocks;
            idJson.push_back(std::move(j));
        }
        result["id"] = std::move(idJson);
        result["json"] = jsonNodes;
        result["repeat"] = repeatNodeData;
        result["command"] = commands;
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
        Profile::push("writing binary cpack to file: {}", path.wstring());
        std::ofstream f(path, std::ios::binary);
        BinaryWriter binaryWriter(true, f);

        //manifest
        binaryWriter.encode(manifest);
        //normal id
        binaryWriter.encode(normalIds);
        //namespace id
        binaryWriter.encode(namespaceIds);
        //item id
        binaryWriter.encodeSize(itemIds->size());
        for (const auto &item: *itemIds) {
            binaryWriter.encode(std::static_pointer_cast<ItemId>(item));
        }
        //block id
        binaryWriter.encodeSize(blockIds->size());
        for (const auto &item: *blockIds) {
            binaryWriter.encode(std::static_pointer_cast<BlockId>(item));
        }
        //json node
        binaryWriter.encode(jsonNodes);
        //repeat node
        binaryWriter.encode(repeatNodeData);
        //command
        binaryWriter.encode(commands);

        f.close();
        Profile::pop();
    }
#endif

    std::shared_ptr<std::vector<std::shared_ptr<NormalId>>>
    CPack::getNormalId(const std::wstring &key) const {
        auto it = normalIds.find(key);
        if (HEDLEY_UNLIKELY(it == normalIds.end())) {
#if CHelperDebug == true
            CHELPER_WARN(R"(fail to find normal ids by key: "{}")", key);
#endif
            return nullptr;
        }
        return it->second;
    }

    std::shared_ptr<std::vector<std::shared_ptr<NamespaceId>>>
    CPack::getNamespaceId(const std::wstring &key) const {
        if (HEDLEY_UNLIKELY(key == L"blocks")) {
            return blockIds;
        } else if (HEDLEY_UNLIKELY(key == L"items")) {
            return itemIds;
        }
        auto it = namespaceIds.find(key);
        if (HEDLEY_UNLIKELY(it == namespaceIds.end())) {
#if CHelperDebug == true
            CHELPER_WARN(R"(fail to find namespace ids by key: "{}")", key);
#endif
            return nullptr;
        }
        return it->second;
    }

}// namespace CHelper
