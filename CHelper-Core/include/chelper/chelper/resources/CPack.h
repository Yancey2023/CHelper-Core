//
// Created by Yancey on 2023/11/7.
//

#pragma once

#ifndef CHELPER_CPACK_H
#define CHELPER_CPACK_H

#include "../node/json/NodeJsonElement.h"
#include "../node/param/NodeCommand.h"
#include "../node/param/NodePerCommand.h"
#include "Manifest.h"
#include "id/BlockId.h"
#include "id/ItemId.h"
#include "pch.h"

namespace CHelper {

    class RepeatData {
    public:
        std::string id;
        std::vector<std::unique_ptr<Node::NodeBase>> breakNodes;
        std::vector<std::vector<std::unique_ptr<Node::NodeBase>>> repeatNodes;
        std::vector<bool> isEnd;
    };

    class CPack {
    public:
        Manifest manifest;
        std::unordered_map<std::string, std::shared_ptr<std::vector<std::shared_ptr<NormalId>>>> normalIds;
        std::unordered_map<std::string, std::shared_ptr<std::vector<std::shared_ptr<NamespaceId>>>> namespaceIds;
        std::shared_ptr<BlockIds> blockIds;
        std::shared_ptr<std::vector<std::shared_ptr<ItemId>>> itemIds;
        std::vector<std::unique_ptr<Node::NodeJsonElement>> jsonNodes;
        std::vector<RepeatData> repeatNodeData;
        std::unordered_map<std::string, std::pair<const RepeatData *, const Node::NodeBase *>> repeatNodes;
        std::shared_ptr<std::vector<std::unique_ptr<Node::NodePerCommand>>> commands = std::make_shared<std::vector<std::unique_ptr<Node::NodePerCommand>>>();
        std::unique_ptr<Node::NodeCommand> mainNode;

    private:
        std::vector<std::unique_ptr<Node::NodeBase>> repeatCacheNodes;

    public:
#ifndef CHELPER_NO_FILESYSTEM
        explicit CPack(const std::filesystem::path &path);
#endif

        explicit CPack(const rapidjson::GenericDocument<rapidjson::UTF8<>> &j);

        explicit CPack(std::istream &binaryReader);

    private:
        void applyId(const rapidjson::GenericValue<rapidjson::UTF8<>> &j);

        void applyJson(const rapidjson::GenericValue<rapidjson::UTF8<>> &j);

        void applyRepeat(const rapidjson::GenericValue<rapidjson::UTF8<>> &j);

        void applyCommand(const rapidjson::GenericValue<rapidjson::UTF8<>> &j) const;

        void afterApply();

    public:
#ifndef CHELPER_NO_FILESYSTEM
        static std::unique_ptr<CPack> createByDirectory(const std::filesystem::path &path);
#endif

        static std::unique_ptr<CPack> createByJson(const rapidjson::GenericDocument<rapidjson::UTF8<>> &j);

        static std::unique_ptr<CPack> createByBinary(std::istream &binaryReader);

#ifndef CHELPER_NO_FILESYSTEM
        void writeJsonToDirectory(const std::filesystem::path &path) const;
#endif

        [[nodiscard]] rapidjson::GenericDocument<rapidjson::UTF8<>> toJson() const;

#ifndef CHELPER_NO_FILESYSTEM
        void writeJsonToFile(const std::filesystem::path &path) const;

        void writeBinToFile(const std::filesystem::path &path) const;
#endif

        [[nodiscard]] std::shared_ptr<std::vector<std::shared_ptr<NormalId>>>
        getNormalId(const std::string &key) const;

        [[nodiscard]] std::shared_ptr<std::vector<std::shared_ptr<NamespaceId>>>
        getNamespaceId(const std::string &key) const;
    };

}// namespace CHelper

CODEC(CHelper::RepeatData, id, breakNodes, repeatNodes, isEnd)

#endif//CHELPER_CPACK_H
