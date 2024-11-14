//
// Created by Yancey on 2023/11/7.
//

#include <chelper/node/NodeType.h>
#include <chelper/resources/CPack.h>
#include <chelper/resources/Manifest.h>

namespace CHelper {

#ifndef CHELPER_NO_FILESYSTEM
    CPack::CPack(const std::filesystem::path &path) {
#ifdef CHelperDebug
        size_t stackSize = Profile::stack.size();
#endif
        Node::currentCreateStage = Node::NodeCreateStage::NONE;
        Profile::push("loading manifest");
        auto jsonManifest = serialization::get_json_from_file(path / "manifest.json");
        serialization::Codec<Manifest>::template from_json(jsonManifest, manifest);
        Profile::next("loading id data");
        for (const auto &file: std::filesystem::recursive_directory_iterator(path / "id")) {
            Profile::next(R"(loading id data in path "{}")", file.path().u16string());
            applyId(serialization::get_json_from_file(file));
        }
        Profile::next("loading json data");
        Node::currentCreateStage = Node::NodeCreateStage::JSON_NODE;
        for (const auto &file: std::filesystem::recursive_directory_iterator(path / "json")) {
            Profile::next(R"(loading json data in path "{}")", file.path().u16string());
            applyJson(serialization::get_json_from_file(file));
        }
        Profile::next("loading repeat data");
        Node::currentCreateStage = Node::NodeCreateStage::REPEAT_NODE;
        for (const auto &file: std::filesystem::recursive_directory_iterator(path / "repeat")) {
            Profile::next(R"(loading repeat data in path "{}")", file.path().u16string());
            applyRepeat(serialization::get_json_from_file(file));
        }
        Profile::next("loading commands");
        Node::currentCreateStage = Node::NodeCreateStage::COMMAND_PARAM_NODE;
        for (const auto &file: std::filesystem::recursive_directory_iterator(path / "command")) {
            Profile::next(R"(loading command in path "{}")", file.path().u16string());
            applyCommand(serialization::get_json_from_file(file));
        }
        Profile::next("init cpack");
        Node::currentCreateStage = Node::NodeCreateStage::NONE;
        afterApply();
        Profile::pop();
#ifdef CHelperDebug
        if (HEDLEY_UNLIKELY(Profile::stack.size() != stackSize)) {
            CHELPER_WARN("error profile stack after loading cpack");
        }
#endif
    }
#endif

    CPack::CPack(const rapidjson::GenericDocument<rapidjson::UTF8<>> &j) {
#ifdef CHelperDebug
        size_t stackSize = Profile::stack.size();
#endif
        Node::currentCreateStage = Node::NodeCreateStage::NONE;
        Profile::push("loading manifest");
        serialization::Codec<Manifest>::template from_json_member(j, "manifest", manifest);
        Profile::next("loading id data");
        for (const auto &item: serialization::find_array_member_or_throw(j, "id")) {
            applyId(item);
        }
        Profile::next("loading json data");
        Node::currentCreateStage = Node::NodeCreateStage::JSON_NODE;
        for (const auto &item: serialization::find_array_member_or_throw(j, "json")) {
            applyJson(item);
        }
        Profile::next("loading repeat data");
        Node::currentCreateStage = Node::NodeCreateStage::REPEAT_NODE;
        for (const auto &item: serialization::find_array_member_or_throw(j, "repeat")) {
            applyRepeat(item);
        }
        Profile::next("loading command data");
        Node::currentCreateStage = Node::NodeCreateStage::COMMAND_PARAM_NODE;
        for (const auto &item: serialization::find_array_member_or_throw(j, "command")) {
            applyCommand(item);
        }
        Profile::next("init cpack");
        Node::currentCreateStage = Node::NodeCreateStage::NONE;
        afterApply();
        Profile::pop();
#ifdef CHelperDebug
        if (HEDLEY_UNLIKELY(Profile::stack.size() != stackSize)) {
            CHELPER_WARN("error profile stack after loading cpack");
        }
#endif
    }

    CPack::CPack(std::istream &istream) {
#ifdef CHelperDebug
        size_t stackSize = Profile::stack.size();
#endif
        Node::currentCreateStage = Node::NodeCreateStage::NONE;
        Profile::push("loading manifest");
        serialization::template from_binary<true>(istream, manifest);
        Profile::next("loading normal id data");
        serialization::template from_binary<true>(istream, normalIds);
        Profile::next("loading namespace id data");
        serialization::template from_binary<true>(istream, namespaceIds);
        Profile::next("loading item id data");
        serialization::template from_binary<true>(istream, itemIds);
        Profile::next("loading block id data");
        serialization::template from_binary<true>(istream, blockIds);
        Profile::next("loading json data");
        Node::currentCreateStage = Node::NodeCreateStage::JSON_NODE;
        serialization::template from_binary<true>(istream, jsonNodes);
        Profile::next("loading repeat data");
        Node::currentCreateStage = Node::NodeCreateStage::REPEAT_NODE;
        serialization::template from_binary<true>(istream, repeatNodeData);
        Profile::next("loading command data");
        Node::currentCreateStage = Node::NodeCreateStage::COMMAND_PARAM_NODE;
        serialization::template from_binary<true>(istream, commands);
        Profile::next("init cpack");
        Node::currentCreateStage = Node::NodeCreateStage::NONE;
        afterApply();
        Profile::pop();
#ifdef CHelperDebug
        if (HEDLEY_UNLIKELY(Profile::stack.size() != stackSize)) {
            CHELPER_WARN("error profile stack after loading cpack");
        }
#endif
    }

    void CPack::applyId(const rapidjson::GenericValue<rapidjson::UTF8<>> &j) {
        std::u16string type;
        serialization::Codec<decltype(type)>::template from_json_member(j, "type", type);
        if (HEDLEY_LIKELY(type == u"normal")) {
            std::u16string id;
            serialization::Codec<decltype(id)>::template from_json_member(j, "id", id);
            std::shared_ptr<std::vector<std::shared_ptr<NormalId>>> content;
            serialization::Codec<decltype(content)>::template from_json_member(j, "content", content);
            normalIds.emplace(std::move(id), std::move(content));
        } else if (HEDLEY_LIKELY(type == u"namespace")) {
            std::u16string id;
            serialization::Codec<decltype(id)>::template from_json_member(j, "id", id);
            std::shared_ptr<std::vector<std::shared_ptr<NamespaceId>>> content;
            serialization::Codec<decltype(content)>::template from_json_member(j, "content", content);
            namespaceIds.emplace(std::move(id), std::move(content));
        } else if (HEDLEY_LIKELY(type == u"block")) {
            serialization::Codec<decltype(blockIds)>::template from_json_member(j, "blocks", blockIds);
        } else if (HEDLEY_LIKELY(type == u"item")) {
            serialization::Codec<decltype(itemIds)>::template from_json_member(j, "items", itemIds);
        } else {
            Profile::push("unknown id type -> {}", type);
            throw std::runtime_error("unknown id type");
        }
    }

    void CPack::applyJson(const rapidjson::GenericValue<rapidjson::UTF8<>> &j) {
        std::unique_ptr<Node::NodeJsonElement> item;
        serialization::Codec<decltype(item)>::template from_json(j, item);
        jsonNodes.push_back(std::move(item));
    }

    void CPack::applyRepeat(const rapidjson::GenericValue<rapidjson::UTF8<>> &j) {
        RepeatData item;
        serialization::Codec<decltype(item)>::template from_json(j, item);
        repeatNodeData.push_back(std::move(item));
    }

    void CPack::applyCommand(const rapidjson::GenericValue<rapidjson::UTF8<>> &j) const {
        std::unique_ptr<Node::NodePerCommand> item;
        serialization::Codec<decltype(item)>::template from_json(j, item);
        commands->push_back(std::move(item));
    }

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
                    u"NODE_REPEAT", u"命令重复部分",
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
            Profile::next(R"(init command: "{}")", StringUtil::join(u",", item->name));
            item->init(*this);
        }
        Profile::next("sort command nodes");
        std::sort(commands->begin(), commands->end(),
                  [](const auto &item1, const auto &item2) {
                      return item1.get()->name[0] < item2.get()->name[0];
                  });
        Profile::next("create main node");
        mainNode = std::make_unique<Node::NodeCommand>(u"MAIN_NODE", u"欢迎使用命令助手(作者：Yancey)", commands.get());
        Profile::pop();
    }

#ifndef CHELPER_NO_FILESYSTEM
    std::unique_ptr<CPack> CPack::createByDirectory(const std::filesystem::path &path) {
        Profile::push("start load CPack by DIRECTORY: {}", path.u16string());
        std::unique_ptr<CPack> cpack = std::make_unique<CPack>(path);
        Profile::pop();
        return cpack;
    }
#endif

    std::unique_ptr<CPack> CPack::createByJson(const rapidjson::GenericDocument<rapidjson::UTF8<>> &j) {
        Profile::push("start load CPack by JSON");
        auto cpack = std::make_unique<CPack>(j);
        Profile::pop();
        return cpack;
    }

    std::unique_ptr<CPack> CPack::createByBinary(std::istream &ifstream) {
        Profile::push("start load CPack by binary");
        std::unique_ptr<CPack> cpack = std::make_unique<CPack>(ifstream);
        Profile::pop();
        return cpack;
    }

#ifndef CHELPER_NO_FILESYSTEM
    void CPack::writeJsonToDirectory(const std::filesystem::path &path) const {
        {
            rapidjson::GenericDocument<rapidjson::UTF8<>> j;
            serialization::Codec<decltype(manifest)>::template to_json(j.GetAllocator(), j, manifest);
            serialization::write_json_to_file(path / "manifest.json", j);
        }
        for (const auto &item: normalIds) {
            rapidjson::GenericDocument<rapidjson::UTF8<>> j;
            j.SetObject();
            j.MemberReserve(3, j.GetAllocator());
            serialization::Codec<decltype(item.first)>::template to_json_member(j.GetAllocator(), j, "id", item.first);
            serialization::Codec<std::string>::template to_json_member(j.GetAllocator(), j, "type", "normal");
            serialization::Codec<decltype(item.second)>::template to_json_member(j.GetAllocator(), j, "content", item.second);
            serialization::write_json_to_file(path / "id" / (item.first + u".json"), j);
        }
        for (const auto &item: namespaceIds) {
            rapidjson::GenericDocument<rapidjson::UTF8<>> j;
            j.SetObject();
            j.MemberReserve(3, j.GetAllocator());
            serialization::Codec<decltype(item.first)>::template to_json_member(j.GetAllocator(), j, "id", item.first);
            serialization::Codec<std::string>::template to_json_member(j.GetAllocator(), j, "type", "namespace");
            serialization::Codec<decltype(item.second)>::template to_json_member(j.GetAllocator(), j, "content", item.second);
            serialization::write_json_to_file(path / "id" / (item.first + u".json"), j);
        }
        {
            rapidjson::GenericDocument<rapidjson::UTF8<>> j;
            j.SetObject();
            j.MemberReserve(3, j.GetAllocator());
            serialization::Codec<std::string>::template to_json_member(j.GetAllocator(), j, "id", "items");
            serialization::Codec<std::string>::template to_json_member(j.GetAllocator(), j, "type", "item");
            serialization::Codec<decltype(itemIds)>::template to_json_member(j.GetAllocator(), j, "items", itemIds);
            serialization::write_json_to_file(path / "id" / "items.json", j);
        }
        {
            rapidjson::GenericDocument<rapidjson::UTF8<>> j;
            j.SetObject();
            j.MemberReserve(3, j.GetAllocator());
            serialization::Codec<std::string>::template to_json_member(j.GetAllocator(), j, "id", "blocks");
            serialization::Codec<std::string>::template to_json_member(j.GetAllocator(), j, "type", "block");
            serialization::Codec<decltype(blockIds)>::template to_json_member(j.GetAllocator(), j, "blocks", blockIds);
            serialization::write_json_to_file(path / "id" / "blocks.json", j);
        }
        for (const auto &item: jsonNodes) {
            rapidjson::GenericDocument<rapidjson::UTF8<>> j;
            serialization::Codec<decltype(item)>::template to_json(j.GetAllocator(), j, item);
            serialization::write_json_to_file(path / "json" / (item->id.value() + u".json"), j);
        }
        for (const auto &item: repeatNodeData) {
            rapidjson::GenericDocument<rapidjson::UTF8<>> j;
            serialization::Codec<decltype(item)>::template to_json(j.GetAllocator(), j, item);
            serialization::write_json_to_file(path / "repeat" / (item.id + u".json"), j);
        }
        for (const auto &item: *commands) {
            rapidjson::GenericDocument<rapidjson::UTF8<>> j;
            serialization::Codec<decltype(item)>::template to_json(j.GetAllocator(), j, item);
            serialization::write_json_to_file(path / "command" / (item->name[0] + u".json"), j);
        }
    }
#endif

    [[nodiscard]] rapidjson::GenericDocument<rapidjson::UTF8<>> CPack::toJson() const {
        rapidjson::GenericDocument<rapidjson::UTF8<>> result;
        result.SetObject();
        serialization::Codec<decltype(manifest)>::template to_json_member(result.GetAllocator(), result, "manifest", manifest);
        rapidjson::GenericValue<rapidjson::UTF8<>> idJson;
        idJson.SetArray();
        idJson.Reserve(normalIds.size() + namespaceIds.size() + 2, result.GetAllocator());
        for (const auto &item: normalIds) {
            rapidjson::GenericValue<rapidjson::UTF8<>> j;
            j.SetObject();
            serialization::Codec<decltype(item.first)>::template to_json_member(result.GetAllocator(), j, "id", item.first);
            serialization::Codec<std::string>::template to_json_member(result.GetAllocator(), j, "type", "normal");
            serialization::Codec<decltype(item.second)>::template to_json_member(result.GetAllocator(), j, "content", item.second);
            idJson.PushBack(std::move(j), result.GetAllocator());
        }
        for (const auto &item: namespaceIds) {
            rapidjson::GenericValue<rapidjson::UTF8<>> j;
            j.SetObject();
            serialization::Codec<decltype(item.first)>::template to_json_member(result.GetAllocator(), j, "id", item.first);
            serialization::Codec<std::string>::template to_json_member(result.GetAllocator(), j, "type", "namespace");
            serialization::Codec<decltype(item.second)>::template to_json_member(result.GetAllocator(), j, "content", item.second);
            idJson.PushBack(std::move(j), result.GetAllocator());
        }
        {
            rapidjson::GenericValue<rapidjson::UTF8<>> j;
            j.SetObject();
            serialization::Codec<std::string>::template to_json_member(result.GetAllocator(), j, "id", "items");
            serialization::Codec<std::string>::template to_json_member(result.GetAllocator(), j, "type", "item");
            serialization::Codec<decltype(itemIds)>::template to_json_member(result.GetAllocator(), j, "items", itemIds);
            idJson.PushBack(std::move(j), result.GetAllocator());
        }
        {
            rapidjson::GenericValue<rapidjson::UTF8<>> j;
            j.SetObject();
            serialization::Codec<std::string>::template to_json_member(result.GetAllocator(), j, "id", "blocks");
            serialization::Codec<std::string>::template to_json_member(result.GetAllocator(), j, "type", "block");
            serialization::Codec<decltype(blockIds)>::template to_json_member(result.GetAllocator(), j, "blocks", blockIds);
            idJson.PushBack(std::move(j), result.GetAllocator());
        }
        result.AddMember(rapidjson::GenericValue<rapidjson::UTF8<>>("id"), std::move(idJson), result.GetAllocator());
        serialization::Codec<decltype(jsonNodes)>::template to_json_member(result.GetAllocator(), result, "json", jsonNodes);
        serialization::Codec<decltype(repeatNodeData)>::template to_json_member(result.GetAllocator(), result, "repeat", repeatNodeData);
        serialization::Codec<decltype(commands)>::template to_json_member(result.GetAllocator(), result, "command", commands);
        return result;
    }

#ifndef CHELPER_NO_FILESYSTEM
    void CPack::writeJsonToFile(const std::filesystem::path &path) const {
        serialization::write_json_to_file(path, toJson());
    }

    void CPack::writeBinToFile(const std::filesystem::path &path) const {
        std::filesystem::create_directories(path.parent_path());
        Profile::push("writing binary cpack to file: {}", path.u16string());
        std::ofstream ostream(path, std::ios::binary);
        //manifest
        serialization::template to_binary<true>(ostream, manifest);
        //normal id
        serialization::template to_binary<true>(ostream, normalIds);
        //namespace id
        serialization::template to_binary<true>(ostream, namespaceIds);
        //item id
        serialization::template to_binary<true>(ostream, itemIds);
        //block id
        serialization::template to_binary<true>(ostream, blockIds);
        //json node
        serialization::template to_binary<true>(ostream, jsonNodes);
        //repeat node
        serialization::template to_binary<true>(ostream, repeatNodeData);
        //command
        serialization::template to_binary<true>(ostream, commands);

        ostream.close();
        Profile::pop();
    }
#endif

    std::shared_ptr<std::vector<std::shared_ptr<NormalId>>>
    CPack::getNormalId(const std::u16string &key) const {
        auto it = normalIds.find(key);
        if (HEDLEY_UNLIKELY(it == normalIds.end())) {
#ifdef CHelperDebug
            CHELPER_WARN(R"(fail to find normal ids by key: "{}")", key);
#endif
            return nullptr;
        }
        return it->second;
    }

    std::shared_ptr<std::vector<std::shared_ptr<NamespaceId>>>
    CPack::getNamespaceId(const std::u16string &key) const {
        if (HEDLEY_UNLIKELY(key == u"blocks")) {
            return std::reinterpret_pointer_cast<std::vector<std::shared_ptr<NamespaceId>>>(blockIds->blockStateValues);
        } else if (HEDLEY_UNLIKELY(key == u"items")) {
            return std::reinterpret_pointer_cast<std::vector<std::shared_ptr<NamespaceId>>>(itemIds);
        }
        auto it = namespaceIds.find(key);
        if (HEDLEY_UNLIKELY(it == namespaceIds.end())) {
#ifdef CHelperDebug
            CHELPER_WARN(R"(fail to find namespace ids by key: "{}")", key);
#endif
            return nullptr;
        }
        return it->second;
    }

}// namespace CHelper
