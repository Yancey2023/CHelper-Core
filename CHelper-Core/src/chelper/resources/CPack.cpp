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
            Profile::next(R"(loading id data in path "{}")", FORMAT_ARG(utf8::utf16to8(file.path().u16string())));
            applyId(serialization::get_json_from_file(file));
        }
        Profile::next("loading json data");
        Node::currentCreateStage = Node::NodeCreateStage::JSON_NODE;
        for (const auto &file: std::filesystem::recursive_directory_iterator(path / "json")) {
            Profile::next(R"(loading json data in path "{}")", FORMAT_ARG(utf8::utf16to8(file.path().u16string())));
            applyJson(serialization::get_json_from_file(file));
        }
        Profile::next("loading repeat data");
        Node::currentCreateStage = Node::NodeCreateStage::REPEAT_NODE;
        for (const auto &file: std::filesystem::recursive_directory_iterator(path / "repeat")) {
            Profile::next(R"(loading repeat data in path "{}")", FORMAT_ARG(utf8::utf16to8(file.path().u16string())));
            applyRepeat(serialization::get_json_from_file(file));
        }
        Profile::next("loading commands");
        Node::currentCreateStage = Node::NodeCreateStage::COMMAND_PARAM_NODE;
        for (const auto &file: std::filesystem::recursive_directory_iterator(path / "command")) {
            Profile::next(R"(loading command in path "{}")", FORMAT_ARG(utf8::utf16to8(file.path().u16string())));
            applyCommand(serialization::get_json_from_file(file));
        }
        Profile::next("init cpack");
        Node::currentCreateStage = Node::NodeCreateStage::NONE;
        afterApply();
        Profile::pop();
#ifdef CHelperDebug
        if (HEDLEY_UNLIKELY(Profile::stack.size() != stackSize)) {
            SPDLOG_WARN("error profile stack after loading cpack");
        }
#endif
    }
#endif

    CPack::CPack(const rapidjson::GenericDocument<rapidjson::UTF8<>> &j) {
        using JsonValueType = rapidjson::GenericDocument<rapidjson::UTF8<>>;
#ifdef CHelperDebug
        size_t stackSize = Profile::stack.size();
#endif
        Node::currentCreateStage = Node::NodeCreateStage::NONE;
        Profile::push("loading manifest");
        serialization::Codec<Manifest>::template from_json_member<JsonValueType>(j, "manifest", manifest);
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
            SPDLOG_WARN("error profile stack after loading cpack");
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
            SPDLOG_WARN("error profile stack after loading cpack");
        }
#endif
    }

    void CPack::applyId(const rapidjson::GenericValue<rapidjson::UTF8<>> &j) {
        using JsonValueType = rapidjson::GenericValue<rapidjson::UTF8<>>;
        std::u16string type;
        serialization::Codec<decltype(type)>::template from_json_member<JsonValueType>(j, "type", type);
        if (HEDLEY_LIKELY(type == u"normal")) {
            std::string id;
            serialization::Codec<decltype(id)>::template from_json_member<JsonValueType>(j, "id", id);
            std::shared_ptr<std::vector<std::shared_ptr<NormalId>>> content;
            serialization::Codec<decltype(content)>::template from_json_member<JsonValueType>(j, "content", content);
            normalIds.emplace(std::move(id), std::move(content));
        } else if (HEDLEY_LIKELY(type == u"namespace")) {
            std::string id;
            serialization::Codec<decltype(id)>::template from_json_member<JsonValueType>(j, "id", id);
            std::shared_ptr<std::vector<std::shared_ptr<NamespaceId>>> content;
            serialization::Codec<decltype(content)>::template from_json_member<JsonValueType>(j, "content", content);
            namespaceIds.emplace(std::move(id), std::move(content));
        } else if (HEDLEY_LIKELY(type == u"block")) {
            serialization::Codec<decltype(blockIds)>::template from_json_member<JsonValueType>(j, "content", blockIds);
        } else if (HEDLEY_LIKELY(type == u"item")) {
            serialization::Codec<decltype(itemIds)>::template from_json_member<JsonValueType>(j, "content", itemIds);
        } else {
            Profile::push("unknown id type -> {}", FORMAT_ARG(utf8::utf16to8(type)));
            throw std::runtime_error("unknown id type");
        }
    }

    void CPack::applyJson(const rapidjson::GenericValue<rapidjson::UTF8<>> &j) {
        using JsonValueType = rapidjson::GenericValue<rapidjson::UTF8<>>;
        std::unique_ptr<Node::NodeJsonElement> item;
        serialization::Codec<decltype(item)>::template from_json<JsonValueType>(j, item);
        jsonNodes.push_back(std::move(item));
    }

    void CPack::applyRepeat(const rapidjson::GenericValue<rapidjson::UTF8<>> &j) {
        using JsonValueType = rapidjson::GenericValue<rapidjson::UTF8<>>;
        RepeatData item;
        serialization::Codec<decltype(item)>::template from_json<JsonValueType>(j, item);
        repeatNodeData.push_back(std::move(item));
    }

    void CPack::applyCommand(const rapidjson::GenericValue<rapidjson::UTF8<>> &j) const {
        using JsonValueType = rapidjson::GenericValue<rapidjson::UTF8<>>;
        std::unique_ptr<Node::NodePerCommand> item;
        serialization::Codec<decltype(item)>::template from_json<JsonValueType>(j, item);
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
                Profile::push("checking repeat node: {}", FORMAT_ARG(item.id));
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
                    auto nodeWrapped = std::make_unique<Node::NodeWrapped>(item3.get());
                    perContent.push_back(nodeWrapped.get());
                    repeatCacheNodes.push_back(std::move(nodeWrapped));
                }
                auto node = std::make_unique<Node::NodeAnd>(std::move(perContent));
                content.push_back(node.get());
                repeatCacheNodes.push_back(std::move(node));
            }
            std::vector<const Node::NodeBase *> breakChildNodes;
            breakChildNodes.reserve(item.breakNodes.size());
            for (const auto &item2: item.breakNodes) {
                auto nodeWrapped = std::make_unique<Node::NodeWrapped>(item2.get());
                breakChildNodes.push_back(nodeWrapped.get());
                repeatCacheNodes.push_back(std::move(nodeWrapped));
            }
            std::unique_ptr<Node::NodeBase> unBreakNode = std::make_unique<Node::NodeOr>(
                    std::move(content), false);
            std::unique_ptr<Node::NodeBase> breakNode = std::make_unique<Node::NodeAnd>(
                    std::move(breakChildNodes));
            std::unique_ptr<Node::NodeBase> orNode = std::make_unique<Node::NodeOr>(
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
            Profile::next(R"(init command: "{}")", FORMAT_ARG(utf8::utf16to8(fmt::format(u"{}", StringUtil::join(item->name, u",")))));
            item->init(*this);
        }
        Profile::next("sort command nodes");
        std::sort(commands->begin(), commands->end(),
                  [](const auto &item1, const auto &item2) {
                      return item1.get()->name[0] < item2.get()->name[0];
                  });
        Profile::next("create main node");
        mainNode = std::make_unique<Node::NodeCommand>("MAIN_NODE", u"欢迎使用命令助手(作者：Yancey)", commands.get());
        Profile::pop();
    }

#ifndef CHELPER_NO_FILESYSTEM
    std::unique_ptr<CPack> CPack::createByDirectory(const std::filesystem::path &path) {
        Profile::push("start load CPack by DIRECTORY: {}", FORMAT_ARG(utf8::utf16to8(path.u16string())));
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

    std::unique_ptr<CPack> CPack::createByBinary(std::istream &istream) {
        Profile::push("start load CPack by binary");
        std::unique_ptr<CPack> cpack = std::make_unique<CPack>(istream);
        Profile::pop();
        return cpack;
    }

#ifndef CHELPER_NO_FILESYSTEM
    template<class JsonType>
    void writeJsonToFileWithCreateDirectory(const std::filesystem::path &path, const JsonType &j) {
        if (!exists(path)) {
            std::filesystem::create_directories(path.parent_path());
        }
        serialization::template write_json_to_file<JsonType>(path, j);
    }
#endif

#ifndef CHELPER_NO_FILESYSTEM
    void CPack::writeJsonToDirectory(const std::filesystem::path &path) const {
        using JsonValueType = rapidjson::GenericDocument<rapidjson::UTF8<>>;
        {
            JsonValueType j;
            serialization::Codec<decltype(manifest)>::template to_json<JsonValueType>(j.GetAllocator(), j, manifest);
            writeJsonToFileWithCreateDirectory<JsonValueType>(path / "manifest.json", j);
        }
        for (const auto &item: normalIds) {
            JsonValueType j;
            j.SetObject();
            j.MemberReserve(3, j.GetAllocator());
            serialization::Codec<decltype(item.first)>::template to_json_member<JsonValueType>(j.GetAllocator(), j, "id", item.first);
            serialization::Codec<std::string>::template to_json_member<JsonValueType>(j.GetAllocator(), j, "type", "normal");
            serialization::Codec<decltype(item.second)>::template to_json_member<JsonValueType>(j.GetAllocator(), j, "content", item.second);
            writeJsonToFileWithCreateDirectory<JsonValueType>(path / "id" / (item.first + ".json"), j);
        }
        for (const auto &item: namespaceIds) {
            JsonValueType j;
            j.SetObject();
            j.MemberReserve(3, j.GetAllocator());
            serialization::Codec<decltype(item.first)>::template to_json_member<JsonValueType>(j.GetAllocator(), j, "id", item.first);
            serialization::Codec<std::string>::template to_json_member<JsonValueType>(j.GetAllocator(), j, "type", "namespace");
            serialization::Codec<decltype(item.second)>::template to_json_member<JsonValueType>(j.GetAllocator(), j, "content", item.second);
            writeJsonToFileWithCreateDirectory<JsonValueType>(path / "id" / (item.first + ".json"), j);
        }
        {
            JsonValueType j;
            j.SetObject();
            j.MemberReserve(3, j.GetAllocator());
            serialization::Codec<std::string>::template to_json_member<JsonValueType>(j.GetAllocator(), j, "id", "item");
            serialization::Codec<std::string>::template to_json_member<JsonValueType>(j.GetAllocator(), j, "type", "item");
            serialization::Codec<decltype(itemIds)>::template to_json_member<JsonValueType>(j.GetAllocator(), j, "content", itemIds);
            writeJsonToFileWithCreateDirectory<JsonValueType>(path / "id" / "items.json", j);
        }
        {
            JsonValueType j;
            j.SetObject();
            j.MemberReserve(3, j.GetAllocator());
            serialization::Codec<std::string>::template to_json_member<JsonValueType>(j.GetAllocator(), j, "id", "block");
            serialization::Codec<std::string>::template to_json_member<JsonValueType>(j.GetAllocator(), j, "type", "block");
            serialization::Codec<decltype(blockIds)>::template to_json_member<JsonValueType>(j.GetAllocator(), j, "content", blockIds);
            writeJsonToFileWithCreateDirectory<JsonValueType>(path / "id" / "block.json", j);
        }
        for (const auto &item: jsonNodes) {
            JsonValueType j;
            serialization::Codec<decltype(item)>::template to_json<JsonValueType>(j.GetAllocator(), j, item);
            writeJsonToFileWithCreateDirectory<JsonValueType>(path / "json" / (item->id.value() + ".json"), j);
        }
        for (const auto &item: repeatNodeData) {
            JsonValueType j;
            serialization::Codec<decltype(item)>::template to_json<JsonValueType>(j.GetAllocator(), j, item);
            writeJsonToFileWithCreateDirectory<JsonValueType>(path / "repeat" / (item.id + ".json"), j);
        }
        for (const auto &item: *commands) {
            JsonValueType j;
            serialization::Codec<decltype(item)>::template to_json<JsonValueType>(j.GetAllocator(), j, item);
            writeJsonToFileWithCreateDirectory<JsonValueType>(path / "command" / (utf8::utf16to8(item->name[0]) + ".json"), j);
        }
    }
#endif

    [[nodiscard]] rapidjson::GenericDocument<rapidjson::UTF8<>> CPack::toJson() const {
        using JsonValueType = rapidjson::GenericDocument<rapidjson::UTF8<>>;
        rapidjson::GenericDocument<rapidjson::UTF8<>> result;
        result.SetObject();
        serialization::Codec<decltype(manifest)>::template to_json_member<JsonValueType>(result.GetAllocator(), result, "manifest", manifest);
        JsonValueType::ValueType idJson;
        idJson.SetArray();
        idJson.Reserve(normalIds.size() + namespaceIds.size() + 2, result.GetAllocator());
        for (const auto &item: normalIds) {
            JsonValueType::ValueType j;
            j.SetObject();
            serialization::Codec<decltype(item.first)>::template to_json_member<typename JsonValueType::ValueType>(result.GetAllocator(), j, "id", item.first);
            serialization::Codec<std::string>::template to_json_member<typename JsonValueType::ValueType>(result.GetAllocator(), j, "type", "normal");
            serialization::Codec<decltype(item.second)>::template to_json_member<typename JsonValueType::ValueType>(result.GetAllocator(), j, "content", item.second);
            idJson.PushBack(std::move(j), result.GetAllocator());
        }
        for (const auto &item: namespaceIds) {
            JsonValueType::ValueType j;
            j.SetObject();
            serialization::Codec<decltype(item.first)>::template to_json_member<typename JsonValueType::ValueType>(result.GetAllocator(), j, "id", item.first);
            serialization::Codec<std::string>::template to_json_member<typename JsonValueType::ValueType>(result.GetAllocator(), j, "type", "namespace");
            serialization::Codec<decltype(item.second)>::template to_json_member<typename JsonValueType::ValueType>(result.GetAllocator(), j, "content", item.second);
            idJson.PushBack(std::move(j), result.GetAllocator());
        }
        {
            JsonValueType::ValueType j;
            j.SetObject();
            serialization::Codec<std::string>::template to_json_member<typename JsonValueType::ValueType>(result.GetAllocator(), j, "id", "item");
            serialization::Codec<std::string>::template to_json_member<typename JsonValueType::ValueType>(result.GetAllocator(), j, "type", "item");
            serialization::Codec<decltype(itemIds)>::template to_json_member<typename JsonValueType::ValueType>(result.GetAllocator(), j, "content", itemIds);
            idJson.PushBack(std::move(j), result.GetAllocator());
        }
        {
            JsonValueType::ValueType j;
            j.SetObject();
            serialization::Codec<std::string>::template to_json_member<typename JsonValueType::ValueType>(result.GetAllocator(), j, "id", "block");
            serialization::Codec<std::string>::template to_json_member<typename JsonValueType::ValueType>(result.GetAllocator(), j, "type", "block");
            serialization::Codec<decltype(blockIds)>::template to_json_member<typename JsonValueType::ValueType>(result.GetAllocator(), j, "content", blockIds);
            idJson.PushBack(std::move(j), result.GetAllocator());
        }
        result.AddMember(rapidjson::GenericValue<rapidjson::UTF8<>>("id"), std::move(idJson), result.GetAllocator());
        serialization::Codec<decltype(jsonNodes)>::template to_json_member<typename JsonValueType::ValueType>(result.GetAllocator(), result, "json", jsonNodes);
        serialization::Codec<decltype(repeatNodeData)>::template to_json_member<typename JsonValueType::ValueType>(result.GetAllocator(), result, "repeat", repeatNodeData);
        serialization::Codec<decltype(commands)>::template to_json_member<typename JsonValueType::ValueType>(result.GetAllocator(), result, "command", commands);
        return result;
    }

#ifndef CHELPER_NO_FILESYSTEM
    void CPack::writeJsonToFile(const std::filesystem::path &path) const {
        writeJsonToFileWithCreateDirectory<rapidjson::GenericDocument<rapidjson::UTF8<>>>(path, toJson());
    }

    void CPack::writeBinToFile(const std::filesystem::path &path) const {
        std::filesystem::create_directories(path.parent_path());
        Profile::push("writing binary cpack to file: {}", FORMAT_ARG(utf8::utf16to8(path.u16string())));
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
    CPack::getNormalId(const std::string &key) const {
        auto it = normalIds.find(key);
        if (HEDLEY_UNLIKELY(it == normalIds.end())) {
#ifdef CHelperDebug
            SPDLOG_WARN(R"(fail to find normal ids by key: "{}")", FORMAT_ARG(key));
#endif
            return nullptr;
        }
        return it->second;
    }

    std::shared_ptr<std::vector<std::shared_ptr<NamespaceId>>>
    CPack::getNamespaceId(const std::string &key) const {
        if (HEDLEY_UNLIKELY(key == "block")) {
            return std::reinterpret_pointer_cast<std::vector<std::shared_ptr<NamespaceId>>>(blockIds->blockStateValues);
        } else if (HEDLEY_UNLIKELY(key == "item")) {
            return std::reinterpret_pointer_cast<std::vector<std::shared_ptr<NamespaceId>>>(itemIds);
        }
        auto it = namespaceIds.find(key);
        if (HEDLEY_UNLIKELY(it == namespaceIds.end())) {
#ifdef CHelperDebug
            SPDLOG_WARN(R"(fail to find namespace ids by key: "{}")", FORMAT_ARG(key));
#endif
            return nullptr;
        }
        return it->second;
    }

}// namespace CHelper
